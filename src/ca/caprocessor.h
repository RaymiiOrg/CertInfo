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

#include "certificate.h"

#include <QSslCertificate>
#include <QVector>
#include <QObject>

class QNetworkReply;

class CAProcessor : public QObject
{
    Q_OBJECT
public:
    explicit CAProcessor(QObject *parent = nullptr);

    static QList<Certificate> getCertificate(const QString& domain);
    static bool isCA(const QSslCertificate& cert);
    
signals:

private slots:
    static void ignoreSslErrors(QNetworkReply* reply);

private:
    static void extractSubject(const QSslCertificate& cert, Certificate& out_issuer);
    static void extractCertificate(const QSslCertificate& cert, Certificate& out_issuer);
};

