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
#include "caprocessor.h"

#include <iostream>
#include <QCoreApplication>
#include <QTimer>
#include <QThread>
#include <QSslCertificateExtension>
#include <QString>
#include <QSslConfiguration>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QEventLoop>

CAProcessor::CAProcessor(QObject *parent)
    : QObject{parent}
{

}

bool CAProcessor::isCA(const QSslCertificate& cert)
{
    for(const auto& ext : cert.extensions()) {
        if(ext.oid() != "2.5.29.19")  // basicConstraints
            continue;

        auto v = ext.value();
        // QVariant containing QVariantMap... Using documentation example to iterate over it...
        if(!v.canConvert<QVariantMap>())
            continue;

        QAssociativeIterable iterable = v.value<QAssociativeIterable>();
        QAssociativeIterable::const_iterator it = iterable.begin();
        const QAssociativeIterable::const_iterator end = iterable.end();
        for ( ; it != end; ++it) {
            if(it.key().toString() == "ca") {
                return it.value().toBool();
            }
        }
    }
    return false;
}

void CAProcessor::ignoreSslErrors(QNetworkReply* reply)
{
    if(reply)
        reply->ignoreSslErrors();
}

QList<Certificate> CAProcessor::getCertificate(const QString& domain)
{
    QList<Certificate> resultList;
    QNetworkAccessManager nam;
    QNetworkRequest request;
    request.setUrl("https://" + domain);
    request.setTransferTimeout(3500);
    request.setRawHeader("User-Agent","Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/106.0.0.0 Safari/537.36 Edg/106.0.1370.52");
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::UserVerifiedRedirectPolicy);

    QEventLoop loop;    
    QNetworkReply* reply = nam.get(request);  

    QTimer::singleShot(4000, &loop, &QEventLoop::quit); // backup timeout
    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, &loop, &QEventLoop::quit);
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&nam, &QNetworkAccessManager::sslErrors, &CAProcessor::ignoreSslErrors);
    loop.exec();

    if(reply && (reply->error() > QNetworkReply::NoError && reply->error() <= QNetworkReply::UnknownNetworkError) ) {
        Certificate error;
        QString errorString = reply->errorString();
        error.subject = domain + ": " + errorString ;
        error.domains.push_back(domain);
        error.errors.push_back(errorString);
        reply->deleteLater();
        return {error};
    }

    QList<QSslCertificate> systemCerts = QSslConfiguration::systemCaCertificates();
    // add last root cert if server did not sent it
    QList<QSslCertificate> peerCertChain = reply->sslConfiguration().peerCertificateChain();
    auto it = std::find_if(systemCerts.begin(), systemCerts.end(), [&peerCertChain](const QSslCertificate& sys){ return peerCertChain.last().issuerDisplayName() == sys.subjectDisplayName(); });
    if(it != systemCerts.end())
        peerCertChain.push_back(*it);

    for(auto it = peerCertChain.begin(); it != peerCertChain.end(); ++it) {
        Certificate result;
        QSslCertificate cert = *it;

        if(cert.isNull())
            continue;

        result.isCA = isCA(cert);

        result.isSelfSigned = cert.isSelfSigned();

        if(std::find(systemCerts.begin(), systemCerts.end(), cert) != systemCerts.end())
            result.isSystemTrustedRootCA = true;


        for(const auto& san : cert.subjectAlternativeNames()) {
            result.subjectAlternativeNames.push_back(san);
        }

        result.domains.push_back(domain);

        result.validFromDate = cert.effectiveDate();
        result.validUntilDate = cert.expiryDate();

        extractSubject(cert, result);
        extractCertificate(cert, result);

        if(!result.subjectAlternativeNames.isEmpty())
            result.isSystemTrustedRootCA = false;

        resultList.push_back(result);
    }


    reply->deleteLater();

    return resultList;
}



void CAProcessor::extractSubject(const QSslCertificate& cert, Certificate& out_issuer)
{
    QString C = cert.subjectInfo(QSslCertificate::CountryName).join(" ");
    if(!C.isEmpty()) {
        out_issuer.subject.append(C + " ");
        out_issuer.subjectInfo.country = C;
    }

    QString ST = cert.subjectInfo(QSslCertificate::StateOrProvinceName).join(" ");
    if(!ST.isEmpty()) {
        out_issuer.subject.append(ST + " ");
        out_issuer.subjectInfo.state = ST;
    }

    QString L = cert.subjectInfo(QSslCertificate::LocalityName).join(" ");
    if(!L.isEmpty()) {
        out_issuer.subject.append(L + " ");
        out_issuer.subjectInfo.locality = L;
    }

    QString O = cert.subjectInfo(QSslCertificate::Organization).join(" ");
    if(!O.isEmpty()) {
        out_issuer.subject.append(O + " ");
        out_issuer.subjectInfo.organization = O;
    }

    QString OU = cert.subjectInfo(QSslCertificate::OrganizationalUnitName).join(" ");
    if(!OU.isEmpty()) {
        out_issuer.subject.append(OU + " ");
        out_issuer.subjectInfo.organizationalUnit = OU;
    }

    QString CN = cert.subjectInfo(QSslCertificate::CommonName).join(" ");
    if(!CN.isEmpty()) {
        out_issuer.subject.append(CN + " ");
        out_issuer.subjectInfo.commonName = CN;
    }

    QString DN = cert.subjectInfo(QSslCertificate::DistinguishedNameQualifier).join(" ");
    if(!DN.isEmpty()) {
        out_issuer.subject.append(DN + " ");
        out_issuer.subjectInfo.distinguishedName = DN;
    }

    QString E = cert.subjectInfo(QSslCertificate::EmailAddress).join(" ");
    if(!E.isEmpty()) {
        out_issuer.subject.append(E + " ");
        out_issuer.subjectInfo.email = E;
    }

    QString SN = cert.subjectInfo(QSslCertificate::SerialNumber).join(" ");
    if(!SN.isEmpty()) {
        out_issuer.subject.append(SN + " ");
        out_issuer.subjectInfo.serial = SN;
    }

}


void CAProcessor::extractCertificate(const QSslCertificate& cert, Certificate& out_issuer)
{
    QString C = cert.issuerInfo(QSslCertificate::CountryName).join(" ");
    if(!C.isEmpty()) {
        out_issuer.issuer.append(C + " ");
        out_issuer.issuerInfo.country = C;
    }

    QString ST = cert.issuerInfo(QSslCertificate::StateOrProvinceName).join(" ");
    if(!ST.isEmpty()) {
        out_issuer.issuer.append(ST + " ");
        out_issuer.issuerInfo.state = ST;
    }

    QString L = cert.issuerInfo(QSslCertificate::LocalityName).join(" ");
    if(!L.isEmpty()) {
        out_issuer.issuer.append(L + " ");
        out_issuer.issuerInfo.locality = L;
    }

    QString O = cert.issuerInfo(QSslCertificate::Organization).join(" ");
    if(!O.isEmpty()) {
        out_issuer.issuer.append(O + " ");
        out_issuer.issuerInfo.organization = O;
    }

    QString OU = cert.issuerInfo(QSslCertificate::OrganizationalUnitName).join(" ");
    if(!OU.isEmpty()) {
        out_issuer.issuer.append(OU + " ");
        out_issuer.issuerInfo.organizationalUnit = OU;
    }

    QString CN = cert.issuerInfo(QSslCertificate::CommonName).join(" ");
    if(!CN.isEmpty()) {
        out_issuer.issuer.append(CN + " ");
        out_issuer.issuerInfo.commonName = CN;
    }

    QString DN = cert.issuerInfo(QSslCertificate::DistinguishedNameQualifier).join(" ");
    if(!DN.isEmpty()) {
        out_issuer.issuer.append(DN + " ");
        out_issuer.issuerInfo.distinguishedName = DN;
    }

    QString E = cert.issuerInfo(QSslCertificate::EmailAddress).join(" ");
    if(!E.isEmpty()) {
        out_issuer.issuer.append(E + " ");
        out_issuer.issuerInfo.email = E;
    }

    QString SN = cert.issuerInfo(QSslCertificate::SerialNumber).join(" ");
    if(!SN.isEmpty()) {
        out_issuer.issuer.append(SN + " ");
        out_issuer.issuerInfo.serial = SN;
    }

}
