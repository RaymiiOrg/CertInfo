#ifndef CACONCURRENTGATHERER_H
#define CACONCURRENTGATHERER_H

#include "src/listmodel/caissuerlistmodel.h"

#include <QString>
#include <QMap>
#include <QObject>

typedef QPair<QString,int> QIntPair;

class CAConcurrentGatherer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList hostnames READ hostnames WRITE setHostnames NOTIFY hostnamesChanged FINAL)
    Q_PROPERTY(CACertificateListModel* issuersCounted READ issuersCounted NOTIFY issuersCountedChanged FINAL)
    Q_PROPERTY(bool busy READ busy WRITE setBusy NOTIFY busyChanged FINAL)
    Q_PROPERTY(QString statusText READ statusText WRITE setStatusText NOTIFY statusTextChanged FINAL)
    Q_PROPERTY(int progress READ progress WRITE setProgress NOTIFY progressChanged FINAL)
    Q_PROPERTY(int privateProgress READ privateProgress WRITE setPrivateProgress NOTIFY privateProgressChanged FINAL)

public:
    explicit CAConcurrentGatherer(QObject *parent = nullptr);

    Q_INVOKABLE void clear();
    Q_INVOKABLE void startGatherCertificatesInBackground();

    QStringList hostnames() const;
    void setHostnames(const QStringList &newHostnames);
    CACertificateListModel *issuersCounted() const;

    bool busy() const;
    void setBusy(bool newBusy);

    QString statusText() const;
    void setStatusText(const QString &newStatusText);

    int progress() const;
    void setProgress(int newProgress);

    int privateProgress() const;
    void setPrivateProgress(int newPrivateProgress);

signals:
    void hostnamesChanged();    
    void issuersCountedChanged();
    void threadBucketFinished();
    void allThreadsFinished();
    void busyChanged();
    void statusTextChanged();
    void progressChanged();
    void privateProgressChanged(int newProgress);

private slots:
    void onThreadBucketFinished();
    void onAllThreadsFinished();

private:
    bool _busy = false;
    QStringList m_hostnames;
    void gatherCertificates();

    QMap<QString, Certificate> resultMap;
    std::vector<Certificate> resultVec;
    CACertificateListModel *m_issuersCounted = nullptr;
    QString m_statusText;
    int m_progress;
    int m_privateProgress;
};

Q_DECLARE_METATYPE(QIntPair)

#endif // CACONCURRENTGATHERER_H
