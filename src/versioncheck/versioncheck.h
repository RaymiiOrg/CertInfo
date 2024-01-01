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

#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QObject>
#include <QRegularExpression>


class VersionCheck : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentVersion READ currentVersion WRITE setCurrentVersion NOTIFY currentVersionChanged)
    Q_PROPERTY(bool newVersionAvailable READ newVersionAvailable NOTIFY newVersionAvailableChanged FINAL)

public:
    explicit VersionCheck(QObject *parent = nullptr);

//properties
    const QString &currentVersion() const;
    bool newVersionAvailable() const;

signals:
    void currentVersionChanged(const QString &currentVersion);
    void newVersionAvailableChanged();

public slots:

    void setCurrentVersion(const QString &newCurrentVersion);
    void slotFinished();
    void getCurrentVersion();
    void setNewVersionAvailable(bool newNewVersionAvailable);

private:
    QNetworkAccessManager* _manager;

    QTimer* _timer;
    QString _currentVersion;
    QUrl _url = QUrl("http://update.leafnode.nl/certinfo.txt");
    void checkVersionOutput(const QString& newVersion);
    bool m_newVersionAvailable;

    static inline const QRegularExpression versionNumberFormat = QRegularExpression("[0-9]{4}\\.[0-9]{2}\\.[0-9]\\.[0-9]"); // 2022.01.0.0



};

