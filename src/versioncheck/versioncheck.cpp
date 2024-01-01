/*
 * Copyright (c) 2022 Remy van Elst
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
 *
 */

#include "VersionCheck.h"
#include <QSettings>
#include <QRandomGenerator>
#include <QGuiApplication>

VersionCheck::VersionCheck(QObject *parent) : QObject(parent)
{
    _manager = new QNetworkAccessManager(this);
    _timer = new QTimer(this);

    int updateCheckInterval = QRandomGenerator::global()->bounded(80000, 300000);

    _timer->setInterval(updateCheckInterval);
    QObject::connect(_timer, &QTimer::timeout, this, &VersionCheck::getCurrentVersion);
    _timer->start();

    QObject::connect(this, &VersionCheck::currentVersionChanged, this, &VersionCheck::checkVersionOutput);

    getCurrentVersion();
}

void VersionCheck::getCurrentVersion()
{
    auto _request = QScopedPointer<QNetworkRequest>(new QNetworkRequest());
    _request->setUrl(_url);
    _request->setTransferTimeout(5000);
    QString ua = "CertInfo v" + QString(APP_VERSION) + " " + QGuiApplication::platformName() + "; ";

    _request->setRawHeader("User-Agent", ua.toUtf8());
    _request->setRawHeader("Version", QString(APP_VERSION).toUtf8());

    QNetworkReply *reply = _manager->get(*_request);
    QObject::connect(reply, &QNetworkReply::finished, this, &VersionCheck::slotFinished);
}

const QString &VersionCheck::currentVersion() const
{
    return _currentVersion;
}

bool VersionCheck::newVersionAvailable() const
{
    return m_newVersionAvailable;
}

void VersionCheck::setCurrentVersion(const QString &newCurrentVersion)
{
    if (_currentVersion == newCurrentVersion)
        return;
    _currentVersion = newCurrentVersion;
    emit currentVersionChanged(_currentVersion);
}

void VersionCheck::slotFinished()
{
    QNetworkReply *reply = dynamic_cast<QNetworkReply*>(sender());
    if(reply != nullptr) {
        QString output  = reply->readAll();
        if(output.contains(versionNumberFormat)) {
            setCurrentVersion(output);
        }
        reply->deleteLater();
    }
}


void VersionCheck::checkVersionOutput(const QString &newVersion)
{
    bool updateAvailable = false;

    QString thisVersionString = APP_VERSION;
    thisVersionString.remove(QChar('.'), Qt::CaseInsensitive);

    QString newVersionString = newVersion;
    newVersionString.remove(QChar('.'), Qt::CaseInsensitive);

    bool thisVersionToLLSuccess = false;
    unsigned long long thisVersionNumber = thisVersionString.toULongLong(&thisVersionToLLSuccess);

    bool newVersionToLLSuccess = false;
    unsigned long long newVersionNumber = newVersionString.toULongLong(&newVersionToLLSuccess);

    if(newVersionToLLSuccess && thisVersionToLLSuccess)
    {
        if(newVersionNumber > thisVersionNumber) {
            updateAvailable = true;
        }
    }

    setNewVersionAvailable(updateAvailable);
}

void VersionCheck::setNewVersionAvailable(bool newNewVersionAvailable)
{
    if (m_newVersionAvailable == newNewVersionAvailable)
        return;
    m_newVersionAvailable = newNewVersionAvailable;
    emit newVersionAvailableChanged();
}
