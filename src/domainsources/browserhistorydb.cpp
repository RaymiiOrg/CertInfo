#include "browserhistorydb.h"

#include <QDir>
#include <QStandardPaths>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QDebug>

BrowserHistoryDb::BrowserHistoryDb(QObject *parent)
    : QObject{parent}
{
    m_domains = new domainCountListModel(this);
}

bool BrowserHistoryDb::openDb(const QUrl path)
{

    _db = QSqlDatabase::addDatabase("QSQLITE");
    setDbFileName(path.toLocalFile());
    _db.setDatabaseName(dbFileName());

    if(!_db.open()) {
        setLastDbError(_db.lastError().text());
        return false;
    } else {
        return true;
    }

}

void BrowserHistoryDb::getHostnamesFromDb()
{
    if(!_db.isOpen())
        return;

    QStringList hostnames;
    QMap<QString, int> countsMap;
    std::vector<QIntPair> countsVec;

    QSqlQuery query;
    if(isFirefox())
        query.prepare(firefoxQuery());
    else
        query.prepare(chromeQuery());

    query.exec();

    while(query.next()) {
        QString host = query.value(0).toString();
        int count = query.value(1).toInt();
        if(isFirefox()) {
            host.chop(1); // remove dot
            std::reverse(host.begin(), host.end());
        }
        hostnames.push_back(host);
        countsMap[host] = count;
    }
    setLastDbError(query.lastError().text());
    setHostnames(hostnames);

    for(auto& [k,v] : countsMap.toStdMap()) {
        QIntPair p(k,v);
        countsVec.push_back(p);
    }

    std::sort(countsVec.begin(), countsVec.end(), [](const QIntPair& a, const QIntPair& b) { return a.second > b.second; });
    m_domains->updateFromVector(countsVec);
}




QStringList BrowserHistoryDb::hostnames() const
{
    return m_hostnames;
}

void BrowserHistoryDb::setHostnames(const QStringList &newHostnames)
{
    if (m_hostnames == newHostnames)
        return;
    m_hostnames = newHostnames;
    emit hostnamesChanged();
}

QString BrowserHistoryDb::dbFileName() const
{
    return m_dbFileName;
}

void BrowserHistoryDb::setDbFileName(const QString &newDbFileName)
{
    if (m_dbFileName == newDbFileName)
        return;
    m_dbFileName = newDbFileName;
    emit dbFileNameChanged();
}

QString BrowserHistoryDb::lastDbError() const
{
    return m_lastDbError;
}

void BrowserHistoryDb::setLastDbError(const QString &newLastDbError)
{
    if (m_lastDbError == newLastDbError)
        return;
    m_lastDbError = newLastDbError;
    emit lastDbErrorChanged();
}

domainCountListModel *BrowserHistoryDb::domains() const
{
    return m_domains;
}

QString BrowserHistoryDb::firefoxQuery()
{
    // Most visited domain names, hostnames are reversed.
    return "SELECT p.rev_host, count(*)"
           " FROM moz_places p"
           " JOIN moz_historyvisits v ON v.place_id = p.id"
           " WHERE p.url LIKE 'https%' "
           " GROUP BY p.rev_host ORDER BY count(*) desc ;";
}

QString BrowserHistoryDb::chromeQuery()
{
    return "SELECT"
           " SUBSTR(SUBSTR(url, INSTR(url, '//') + 2), 0, INSTR(SUBSTR(url, INSTR(url, '//') + 2), '/'))  AS domain,"
           " SUM(visit_count) AS total_visits"
           " FROM urls"
           " WHERE url LIKE 'https%'"
           " GROUP BY domain"
           " ORDER BY total_visits DESC;";
}

bool BrowserHistoryDb::isFirefox() const
{
    return m_isFirefox;
}

void BrowserHistoryDb::setIsFirefox(bool newIsFirefox)
{
    if (m_isFirefox == newIsFirefox)
        return;
    m_isFirefox = newIsFirefox;
    emit isFirefoxChanged();
}

QUrl BrowserHistoryDb::firefoxFolder() const
{
#ifdef Q_OS_WIN
    QString appdataPath = QStandardPaths::standardLocations(QStandardPaths::GenericConfigLocation).at(0) + "/Roaming/Mozilla/Firefox/Profiles";
    return QUrl::fromLocalFile(appdataPath);
#else
    return QUrl::fromLocalFile(QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0));
#endif
}

QUrl BrowserHistoryDb::chromeFolder() const
{
#ifdef Q_OS_WIN
    QString appdataPath = QStandardPaths::standardLocations(QStandardPaths::GenericConfigLocation).at(0) + "/Google/Chrome/User Data";
    return QUrl::fromLocalFile(appdataPath);
#else
    return QUrl::fromLocalFile(QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0));
#endif
}
