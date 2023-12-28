#ifndef DOMAINSLISTTEXTFILE_H
#define DOMAINSLISTTEXTFILE_H

#include "src/listmodel/domaincountlistmodel.h"

#include <QMap>
#include <QUrl>
#include <QObject>

typedef QPair<QString,int> QIntPair;


class DomainsListTextFile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(domainCountListModel* domains READ domains NOTIFY domainsChanged FINAL)
    Q_PROPERTY(QStringList hostnames READ hostnames WRITE setHostnames NOTIFY hostnamesChanged FINAL)
    Q_PROPERTY(QString textFileName READ textFileName WRITE setTextFileName NOTIFY textFileNameChanged FINAL)
    Q_PROPERTY(QString lastError READ lastError WRITE setLastError NOTIFY lastErrorChanged FINAL)

public:
    explicit DomainsListTextFile(QObject *parent = nullptr);

    Q_INVOKABLE void getHostnamesFromTextFile(const QUrl path);

    QStringList hostnames() const;

    QString textFileName() const;
    void setTextFileName(const QString &newTextFileName);

    domainCountListModel *domains() const;

    QString lastError() const;
    void setLastError(const QString &newLastError);

    void setHostnames(const QStringList &newHostnames);

signals:
    void hostnamesChanged();
    void textFileNameChanged();

    void domainsChanged();

    void lastErrorChanged();

private:
    QStringList m_hostnames;
    QString m_textFileName;
    domainCountListModel *m_domains = nullptr;
    QString m_lastError;
};

#endif // DOMAINSLISTTEXTFILE_H
