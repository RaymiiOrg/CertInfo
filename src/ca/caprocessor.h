#ifndef CAPROCESSOR_H
#define CAPROCESSOR_H

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

    static QVector<Certificate> getCertificate(const QString& domain);
    static bool isCA(const QSslCertificate& cert);
    
signals:

private slots:
    static void ignoreSslErrors(QNetworkReply* reply);

private:
    static void extractSubject(const QSslCertificate& cert, Certificate& out_issuer);
    static void extractCertificate(const QSslCertificate& cert, Certificate& out_issuer);
};

#endif // CAPROCESSOR_H
