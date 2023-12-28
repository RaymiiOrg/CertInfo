#ifndef BROWSERHISTORYDB_H
#define BROWSERHISTORYDB_H

#include "src/listmodel/domaincountlistmodel.h"

#include <QMap>
#include <QUrl>
#include <QObject>
#include <QSqlDatabase>

typedef QPair<QString,int> QIntPair;

class BrowserHistoryDb : public QObject
{
    Q_OBJECT
    Q_PROPERTY(domainCountListModel* domains READ domains NOTIFY domainsChanged FINAL)
    Q_PROPERTY(QStringList hostnames READ hostnames WRITE setHostnames NOTIFY hostnamesChanged FINAL)
    Q_PROPERTY(QString dbFileName READ dbFileName WRITE setDbFileName NOTIFY dbFileNameChanged FINAL)
    Q_PROPERTY(bool isFirefox READ isFirefox WRITE setIsFirefox NOTIFY isFirefoxChanged FINAL)
    Q_PROPERTY(QString lastDbError READ lastDbError WRITE setLastDbError NOTIFY lastDbErrorChanged FINAL)
    Q_PROPERTY(QUrl firefoxFolder READ firefoxFolder CONSTANT FINAL)
    Q_PROPERTY(QUrl chromeFolder READ chromeFolder CONSTANT FINAL)

public:
    explicit BrowserHistoryDb(QObject *parent = nullptr);

    Q_INVOKABLE bool openDb(const QUrl path);
    Q_INVOKABLE void getHostnamesFromDb();

    QStringList hostnames() const;
    void setHostnames(const QStringList &newHostnames);

    QString dbFileName() const;
    void setDbFileName(const QString &newDbFileName);

    QString lastDbError() const;
    void setLastDbError(const QString &newLastDbError);

    domainCountListModel *domains() const;

    bool isFirefox() const;
    void setIsFirefox(bool newIsFirefox);

    QUrl firefoxFolder() const;

    QUrl chromeFolder() const;

signals:

    void hostnamesChanged();
    void dbFileNameChanged();
    void lastDbErrorChanged();    
    void domainsChanged();
    void isFirefoxChanged();

private:
    QSqlDatabase _db;
    QString firefoxQuery();
    QString chromeQuery();
    QStringList m_hostnames;
    QString m_dbFileName;
    QString m_lastDbError;
    domainCountListModel *m_domains = nullptr;
    bool m_isFirefox;
};

#endif // BROWSERHISTORYDB_H
