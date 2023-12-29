/*
 * Copyright (c) 2023 Remy van Elst https://raymii.org
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include "caconcurrentgatherer.h"
#include "caprocessor.h"

#include <iostream>
#include <QtConcurrent/QtConcurrent>
#include <QThreadPool>
#include <QFuture>
#include <QCoreApplication>

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
    if(busy()) {
        setStop(true);
        return;
    }

    setBusy(true);
    QtConcurrent::run([this]() {
        QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, [&](){setStop(true);});
        m_issuersCounted->clear();        
        setPrivateProgress(0);
        gatherCertificates();
    });

}

void CAConcurrentGatherer::exportToText(const QUrl &path)
{
    if(busy())
        return;


    QFile file(path.toLocalFile());
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        QList<Certificate> result;
        QHash<QString, Certificate>::iterator i;
        for(i = resultHash.begin(); i != resultHash.end(); ++i)
            result.push_back(i.value());

        std::sort(result.begin(), result.end(), [](const Certificate& a, const Certificate& b) { return a.count > b.count; });

        for(const auto& r : result) {
            stream << r.toQString() << "\n";
            stream << "==============================\n\n";
        }

        file.close();
    }

}

void CAConcurrentGatherer::gatherCertificates()
{
    resultHash.clear();

    int bucketSize = 10;
    int totalSize = m_hostnames.size();
    int amountOfBuckets = (totalSize ) / bucketSize;
    int currentCounter = 0;
    if(amountOfBuckets < 1) {
        amountOfBuckets = 1;
        bucketSize = totalSize;
    }

    QThreadPool pool;
    for(int i = 0; i < amountOfBuckets; ++i) {
        if(m_stop)
            break;

        // gather the certificates concurrently, but only 10 at once
        QFutureSynchronizer<QList<Certificate>> synchronizer;
        int thisBucketEndsAt = bucketSize + currentCounter;
        setStatusText("Checking domains " + QString::number(currentCounter) + " to " + QString::number(thisBucketEndsAt) + " (of " + QString::number(m_hostnames.size()) + ")");
        for(int j = currentCounter; j < thisBucketEndsAt; ++j) {
            synchronizer.addFuture(QtConcurrent::run(&pool, CAProcessor::getCertificate, m_hostnames.at(currentCounter)));
            ++currentCounter;
        }
        synchronizer.waitForFinished();

        QList<QFuture<QList<Certificate>>> futures = synchronizer.futures();
        for(int i = 0; i < futures.count(); ++i) {
            QList<Certificate> rV = futures.at(i).result();
            for(Certificate& r : rV) {
                if(resultHash.contains(r.subject)) {
                    for(const auto& d : qAsConst(r.domains))
                        resultHash[r.subject].domains.push_back(d);
                } else {
                    resultHash[r.subject] = r;
                }
                ++resultHash[r.subject].count;
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
    setStatusText("Finished all domains");
    setPrivateProgress(100);
    setBusy(false);
    setStop(false);
}

void CAConcurrentGatherer::onThreadBucketFinished()
{

    resultList.clear();

    QHash<QString, Certificate>::iterator i;
    for(i = resultHash.begin(); i != resultHash.end(); ++i)
        resultList.push_back(i.value());

    // sort by count
    // std::sort(resultList.begin(), resultList.end(), [](const Certificate& a, const Certificate& b) { return a.count > b.count; });

    for(const Certificate& c : resultList) {
        m_issuersCounted->addOrUpdateItem(c, c.subject);
    }

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

bool CAConcurrentGatherer::stop() const
{
    return m_stop;
}

void CAConcurrentGatherer::setStop(bool newStop)
{
    if (m_stop == newStop)
        return;

    m_stop = newStop;
    emit stopChanged();
}
