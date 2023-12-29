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

#pragma once

#include "src/listmodel/caissuerlistmodel.h"

#include <atomic>
#include <QMutex>
#include <QString>
#include <QMap>
#include <QObject>

typedef QPair<QString,int> QIntPair;

class CAConcurrentGatherer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool stop READ stop WRITE setStop NOTIFY stopChanged FINAL)
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
    Q_INVOKABLE void exportToText(const QUrl& path);

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

    bool stop() const;
    void setStop(bool newStop);

signals:
    void hostnamesChanged();    
    void issuersCountedChanged();
    void threadBucketFinished();
    void allThreadsFinished();
    void busyChanged();
    void statusTextChanged();
    void progressChanged();
    void privateProgressChanged(int newProgress);

    void stopChanged();

private slots:
    void onThreadBucketFinished();
    void onAllThreadsFinished();

private:
    bool _busy = false;
    QStringList m_hostnames;
    void gatherCertificates();
    QHash<QString, Certificate> resultHash;
    QList<Certificate> resultList;
    CACertificateListModel *m_issuersCounted = nullptr;
    QString m_statusText;
    int m_progress;
    int m_privateProgress;
    std::atomic<bool> m_stop = false;
};

Q_DECLARE_METATYPE(QIntPair)
