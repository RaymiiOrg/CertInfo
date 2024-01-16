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


#include "caissuerlistmodel.h"

CACertificateListModel::CACertificateListModel(QObject* parent) : GenericListModel<Certificate>(parent)
{
    addSelector("subject", [](const Certificate &i) { return i.subject; });
    addSelector("subjectInfo", [](const Certificate &i) { return QVariant::fromValue(i.subjectInfo); });
    addSelector("string", [](const Certificate &i) { return QString::fromStdString(i.toString()); });
    addSelector("issuer", [](const Certificate &i) { return i.issuer; });
    addSelector("issuerInfo", [](const Certificate &i) { return QVariant::fromValue(i.issuerInfo); });
    addSelector("validFromDate", [](const Certificate &i) { return i.validFromDate.toString(); });
    addSelector("validUntilDate", [](const Certificate &i) { return i.validUntilDate.toString(); });
    addSelector("count", [](const Certificate &i) { return i.count; });
    addSelector("isca", [](const Certificate &i) { return i.isCA; });
    addSelector("istrustedrootca", [](const Certificate &i) { return i.isSystemTrustedRootCA; });
    addSelector("domains", [](const Certificate &i) { return i.domains.join(" "); });
    addSelector("subjectAlternativeNames", [](const Certificate &i) { return i.subjectAlternativeNames.join(" "); });
    addSelector("isselfsigned", [](const Certificate &i) { return i.isSelfSigned; });
    addSelector("errors", [](const Certificate &i) { return i.errors.join(" "); });
}

void CACertificateListModel::addOrUpdateItem(const Certificate &item, const QString& findBySubject)
{
    auto it = std::find_if(m_listObjects.begin(), m_listObjects.end(), [&findBySubject](const Certificate& c){ return c.subject == findBySubject;});
    if(it != m_listObjects.end())
    {
        int index = std::distance(m_listObjects.begin(), it);
        updateRow(index, item);
    } else {
        addRow(item);
    }
}
