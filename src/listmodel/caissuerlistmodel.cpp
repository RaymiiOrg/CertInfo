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
