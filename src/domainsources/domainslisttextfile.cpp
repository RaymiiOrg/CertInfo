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
    QMap<QString,int> countsHash;
    QList<QIntPair> countsList;

    QFile file(textFileName());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setLastError(file.errorString());
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if(!line.isEmpty()) {
            countsHash[line]++;            
        }
    }

    file.close();

    QMap<QString, int>::iterator i;
    for(i = countsHash.begin(); i != countsHash.end(); ++i) {
        countsList.push_back({i.key(), i.value()});
    }

    std::sort(countsList.begin(), countsList.end(), [](const QIntPair& a, const QIntPair& b) { return a.second > b.second; });

    for(i = countsHash.begin(); i != countsHash.end(); ++i) {
        hostnames.push_back(i.key());
    }
    setHostnames(hostnames);
    m_domains->updateFromQList(countsList);
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
