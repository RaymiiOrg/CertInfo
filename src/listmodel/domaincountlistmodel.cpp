#include "domaincountlistmodel.h"

domainCountListModel::domainCountListModel(QObject* parent) : GenericListModel<QIntPair>(parent)
{
    addValueSelector("domain", [](const QIntPair &m) { return m.first; });
    addValueSelector("count", [](const QIntPair &m) { return m.second; });
}
