#include "domainslisttextfile.h"
#include <QFile>
#include <QTextStream>
#include <QMap>

DomainsListTextFile::DomainsListTextFile(QObject *parent)
    : QObject{parent}
{
    m_domains = new domainCountListModel(this);
}

void DomainsListTextFile::getHostnamesFromTextFile(const QUrl path)
{
    setTextFileName(path.toLocalFile());

    QStringList hostnames;
    QMap<QString,int> countsMap;
    std::vector<QIntPair> countsVec;

    QFile file(textFileName());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setLastError(file.errorString());
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if(!line.isEmpty()) {
            countsMap[line]++;
            hostnames.push_back(line);
        }
    }

    file.close();

    setHostnames(hostnames);

    for(auto& [k,v] : countsMap.toStdMap()) {
        QIntPair p(k,v);
        countsVec.push_back(p);
    }

    std::sort(countsVec.begin(), countsVec.end(), [](const QIntPair& a, const QIntPair& b) { return a.second > b.second; });
    m_domains->updateFromVector(countsVec);
}

QStringList DomainsListTextFile::hostnames() const
{
    return m_hostnames;
}

QString DomainsListTextFile::textFileName() const
{
    return m_textFileName;
}

void DomainsListTextFile::setTextFileName(const QString &newTextFileName)
{
    if (m_textFileName == newTextFileName)
        return;
    m_textFileName = newTextFileName;
    emit textFileNameChanged();
}

domainCountListModel *DomainsListTextFile::domains() const
{
    return m_domains;
}

QString DomainsListTextFile::lastError() const
{
    return m_lastError;
}

void DomainsListTextFile::setLastError(const QString &newLastError)
{
    if (m_lastError == newLastError)
        return;
    m_lastError = newLastError;
    emit lastErrorChanged();
}

void DomainsListTextFile::setHostnames(const QStringList &newHostnames)
{
    if (m_hostnames == newHostnames)
        return;
    m_hostnames = newHostnames;
    emit hostnamesChanged();
}
