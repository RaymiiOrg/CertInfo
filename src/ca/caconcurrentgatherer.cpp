#include "caconcurrentgatherer.h"
#include "caprocessor.h"

#include <iostream>
#include <QtConcurrent/QtConcurrent>
#include <QThreadPool>
#include <QFuture>

CAConcurrentGatherer::CAConcurrentGatherer(QObject *parent)
    : QObject{parent}
{
    m_issuersCounted = new CACertificateListModel(this);
    /* emitting hostnames changed from a different thread makes QML complain:
     * QObject::connect: Cannot queue arguments of type 'QQmlChangeSet'
     * (Make sure 'QQmlChangeSet' is registered using qRegisterMetaType().)
     */
    connect(this, &CAConcurrentGatherer::threadBucketFinished, this, &CAConcurrentGatherer::onThreadBucketFinished, Qt::QueuedConnection);
    connect(this, &CAConcurrentGatherer::allThreadsFinished, this, &CAConcurrentGatherer::onAllThreadsFinished, Qt::QueuedConnection);
    connect(this, &CAConcurrentGatherer::privateProgressChanged, this, &CAConcurrentGatherer::setProgress, Qt::QueuedConnection);

    setPrivateProgress(0);
}

void CAConcurrentGatherer::clear()
{
    if(!busy()) {
        setHostnames({});
        m_issuersCounted->clear();
    }
}

void CAConcurrentGatherer::startGatherCertificatesInBackground()
{
    // run on a background thread or the UI will hang
    if(busy())
        return;

    setBusy(true);
    QtConcurrent::run([this]() {
        m_issuersCounted->clear();
        setPrivateProgress(0);
        gatherCertificates();
    });

}

void CAConcurrentGatherer::gatherCertificates()
{
    QThreadPool pool;
    // check 10 domains at once
    int bucketSize = 10;
    int totalSize = m_hostnames.size();
    int amountOfBuckets = (totalSize ) / bucketSize;
    int currentCounter = 0;
    if(amountOfBuckets < 1) {
        amountOfBuckets = 1;
        bucketSize = totalSize;
    }

    for(int i = 0; i < amountOfBuckets; ++i) {
        // gather the certificates concurrently, but only 10 at once
        QFutureSynchronizer<QVector<Certificate>> synchronizer;
        int thisBucketEndsAt = bucketSize + currentCounter;
        setStatusText("Checking domains " + QString::number(currentCounter) + " to " + QString::number(thisBucketEndsAt) + " (of " + QString::number(m_hostnames.size()) + ")");
        for(int j = currentCounter; j < thisBucketEndsAt; ++j) {
            synchronizer.addFuture(QtConcurrent::run(&pool, CAProcessor::getCertificate, m_hostnames.at(currentCounter)));
            ++currentCounter;
        }
        synchronizer.waitForFinished();

        std::vector<Certificate> tmpResult;
        QList<QFuture<QVector<Certificate>>> futures = synchronizer.futures();
        for(int i = 0; i < futures.count(); ++i) {
            auto rV = futures.at(i).result();
            for(auto r : rV) {
                if(resultMap.contains(r.subject)) {
                    for(const auto& d : r.domains)
                        resultMap[r.subject].domains.push_back(d);
                } else {
                    resultMap[r.subject] = r;
                }
                ++resultMap[r.subject].count;
            }
        }


        int currentPercent = ((double)currentCounter*100/(double)totalSize);
        setPrivateProgress(currentPercent);
        emit threadBucketFinished();
    }

    emit allThreadsFinished();
}



void CAConcurrentGatherer::onAllThreadsFinished()
{
    resultMap.clear();
    setStatusText("Finished all domains");
    setPrivateProgress(100);
    setBusy(false);
}

void CAConcurrentGatherer::onThreadBucketFinished()
{

    resultVec.clear();

    for(const auto& [k,v] : resultMap.toStdMap())
        resultVec.push_back(v);

    // sort by count
    std::sort(resultVec.begin(), resultVec.end(), [](const Certificate& a, const Certificate& b) { return a.count > b.count; });


    m_issuersCounted->updateFromVector(resultVec);
}


bool CAConcurrentGatherer::busy() const
{
    return _busy;
}

void CAConcurrentGatherer::setBusy(bool newBusy)
{
    if (_busy == newBusy)
        return;
    _busy = newBusy;
    emit busyChanged();
}



QStringList CAConcurrentGatherer::hostnames() const
{
    return m_hostnames;
}

void CAConcurrentGatherer::setHostnames(const QStringList &newHostnames)
{
    if (m_hostnames == newHostnames)
        return;
    m_hostnames = newHostnames;
    emit hostnamesChanged();
}

CACertificateListModel *CAConcurrentGatherer::issuersCounted() const
{
    return m_issuersCounted;
}

QString CAConcurrentGatherer::statusText() const
{
    return m_statusText;
}

void CAConcurrentGatherer::setStatusText(const QString &newStatusText)
{
    if (m_statusText == newStatusText)
        return;
    m_statusText = newStatusText;
    emit statusTextChanged();
}

int CAConcurrentGatherer::progress() const
{
    return m_progress;
}

void CAConcurrentGatherer::setProgress(int newProgress)
{
    if (m_progress == newProgress)
        return;
    m_progress = newProgress;
    emit progressChanged();
}

int CAConcurrentGatherer::privateProgress() const
{
    return m_privateProgress;
}

void CAConcurrentGatherer::setPrivateProgress(int newPrivateProgress)
{
    if (m_privateProgress == newPrivateProgress)
        return;
    m_privateProgress = newPrivateProgress;
    emit privateProgressChanged(newPrivateProgress);
}
