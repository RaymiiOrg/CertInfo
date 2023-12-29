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
    addValueSelector("subject", [](const Certificate &i) { return i.subject; });
    addValueSelector("subjectInfo", [](const Certificate &i) { return QVariant::fromValue(i.subjectInfo); });
    addValueSelector("string", [](const Certificate &i) { return QString::fromStdString(i.toString()); });
    addValueSelector("issuer", [](const Certificate &i) { return i.issuer; });
    addValueSelector("issuerInfo", [](const Certificate &i) { return QVariant::fromValue(i.issuerInfo); });
    addValueSelector("validFromDate", [](const Certificate &i) { return i.validFromDate.toString(); });
    addValueSelector("validUntilDate", [](const Certificate &i) { return i.validUntilDate.toString(); });
    addValueSelector("count", [](const Certificate &i) { return i.count; });
    addValueSelector("isca", [](const Certificate &i) { return i.isCA; });
    addValueSelector("istrustedrootca", [](const Certificate &i) { return i.isSystemTrustedRootCA; });
    addValueSelector("domains", [](const Certificate &i) { return i.domains.join(" "); });
    addValueSelector("subjectAlternativeNames", [](const Certificate &i) { return i.subjectAlternativeNames.join(" "); });
    addValueSelector("isselfsigned", [](const Certificate &i) { return i.isSelfSigned; });
    addValueSelector("errors", [](const Certificate &i) { return i.errors.join(" "); });
}

void CACertificateListModel::addOrUpdateItem(const Certificate &item, const QString& findBySubject)
{
    auto it = std::find_if(_listObjects.begin(), _listObjects.end(), [&findBySubject](const Certificate& c){ return c.subject == findBySubject;});
    if(it != _listObjects.end())
    {
        int index = std::distance(_listObjects.begin(), it);
        updateRow(index, item);
    } else {
        addRow(item);
    }
}
