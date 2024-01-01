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

#include <sstream>
#include <QObject>
#include <QDateTime>
#include <QString>
#include <QSslCertificate>
#include <QStringList>

struct SubjectInfo {
    Q_GADGET
    Q_PROPERTY(QString country MEMBER country)
    Q_PROPERTY(QString state MEMBER state)
    Q_PROPERTY(QString locality MEMBER locality)
    Q_PROPERTY(QString organization MEMBER organization)
    Q_PROPERTY(QString organizationalUnit MEMBER organizationalUnit)
    Q_PROPERTY(QString commonName MEMBER commonName)
    Q_PROPERTY(QString distinguishedName MEMBER distinguishedName)
    Q_PROPERTY(QString email MEMBER email)
    Q_PROPERTY(QString serial MEMBER serial)

public:
    QString country;
    QString state;
    QString locality;
    QString organization;
    QString organizationalUnit;
    QString commonName;
    QString distinguishedName;
    QString email;
    QString serial;




    QString toQString() const {
        return QString::fromStdString(toString());
    }

    std::string toString() const {
        std::string result;
        if(!country.isEmpty())
            result.append("C  : " + country.toStdString() + "\n");
        if(!state.isEmpty())
            result.append("ST : " + state.toStdString() + "\n");
        if(!locality.isEmpty())
            result.append("L  : " + locality.toStdString() + "\n");
        if(!organization.isEmpty())
            result.append("O  : " + organization.toStdString() + "\n");
        if(!organizationalUnit.isEmpty())
            result.append("OU : " + organizationalUnit.toStdString() + "\n");
        if(!commonName.isEmpty())
            result.append("CN : " + commonName.toStdString() + "\n");
        if(!distinguishedName.isEmpty())
            result.append("DN : " + distinguishedName.toStdString() + "\n");
        if(!email.isEmpty())
            result.append("E  : " + email.toStdString() + "\n");
        if(!serial.isEmpty())
            result.append("SN : " + serial.toStdString() + "\n");

        if(result.empty())
            result.append("Empty Subject\n");


        return result;
    }

    friend std::ostream& operator<<(std::ostream& os, const SubjectInfo& subjectInfo) {
        os << subjectInfo.toString();
        return os;
    }

    bool operator==(const SubjectInfo& other) const {
        return (
            country == other.country &&
            state == other.state &&
            locality == other.locality &&
            organization == other.organization &&
            organizationalUnit == other.organizationalUnit &&
            commonName == other.commonName &&
            distinguishedName == other.distinguishedName &&
            email == other.email &&
            serial == other.serial
            );
    }

    bool operator!=(const SubjectInfo& other) const {
        return !(*this == other);
    }

    bool operator<(const SubjectInfo& other) const {
        // Compare each member lexicographically
        return (
            country < other.country ||
            (country == other.country && state < other.state) ||
            (country == other.country && state == other.state && locality < other.locality) ||
            (country == other.country && state == other.state && locality == other.locality &&
             organization < other.organization) ||
            (country == other.country && state == other.state && locality == other.locality &&
             organization == other.organization && organizationalUnit < other.organizationalUnit) ||
            (country == other.country && state == other.state && locality == other.locality &&
             organization == other.organization && organizationalUnit == other.organizationalUnit &&
             commonName < other.commonName) ||
            (country == other.country && state == other.state && locality == other.locality &&
             organization == other.organization && organizationalUnit == other.organizationalUnit &&
             commonName == other.commonName && distinguishedName < other.distinguishedName) ||
            (country == other.country && state == other.state && locality == other.locality &&
             organization == other.organization && organizationalUnit == other.organizationalUnit &&
             commonName == other.commonName && distinguishedName == other.distinguishedName &&
             email < other.email) ||
            (country == other.country && state == other.state && locality == other.locality &&
             organization == other.organization && organizationalUnit == other.organizationalUnit &&
             commonName == other.commonName && distinguishedName == other.distinguishedName &&
             email == other.email && serial < other.serial)
            );
    }
};
Q_DECLARE_METATYPE(SubjectInfo);

struct Certificate {
    Q_GADGET
public:
    QString subject;
    SubjectInfo subjectInfo;
    QString issuer;
    SubjectInfo issuerInfo;
    QDateTime validFromDate;
    QDateTime validUntilDate;
    int count = 0;
    bool isSelfSigned = false;
    bool isCA = false;
    bool isSystemTrustedRootCA = false;
    QStringList domains; // domain that was in user provided history
    QStringList subjectAlternativeNames; // all domains that cert has
    QStringList errors;
    QSslCertificate _actualCert;

    bool operator==(const Certificate& other) const {
        return (subject == other.subject);
    }

    bool operator!=(const Certificate& other) const {
        return !(*this == other);
    }

    bool operator<(const Certificate& other) const {
        if(subject == other.subject)
            return count < other.count;
        else
            return subject.localeAwareCompare(other.subject);
    }

    QString toQString() const {
        return QString::fromStdString(toString());
    }

    std::string toString() const {
        std::stringstream ss;
        ss << "Count: " << count << "; \n"
           << "Subject: " << subject.toStdString() << "; \n";
        ss << subjectInfo.toString() << "\n\n";

        ss << "Valid From: " << validFromDate.toString().toStdString() << "\n";
        ss << "Valid Until:" << validUntilDate.toString().toStdString() << "\n";

        ss << "Issuer: " << issuer.toStdString() << "; \n";
        ss << issuerInfo.toString() << "\n\n";

        ss << "CA: " << (isCA ? "true" : "false") <<  "\n";

        ss << "Self Signed: " << (isSelfSigned ? "true" : "false") <<  "\n";

        ss << "Trusted Root CA:" << (isSystemTrustedRootCA ? "true" : "false") <<  "\n";

        if(!domains.isEmpty()) {
            ss << "User Domains: ";

            for (const auto& domain : domains) {
                ss << domain.toStdString() << " ";
            }
            ss << "; \n";
        }

        if(!subjectAlternativeNames.isEmpty()) {
        ss << "subjectAltNames: ";

            for (const auto& san : subjectAlternativeNames) {
                ss << san.toStdString() << " ";
            }
            ss << "; \n";
        }

        if(!errors.isEmpty()) {
            ss << "Errors:         ";

            for (const auto& err : errors) {
                ss << err.toStdString() << " ";
            }
            ss << "; \n";
        }

        return ss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const Certificate& issuer) {
        os << issuer.toString();
        return os;
    }

};

Q_DECLARE_METATYPE(Certificate);
