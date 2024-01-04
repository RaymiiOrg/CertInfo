#include "caissuerfilter.h"
#include <QDebug>

CAIssuerFilter::CAIssuerFilter(QObject *parent) :
    QSortFilterProxyModelWithRowCountSignal(parent)
{
}

void CAIssuerFilter::setFilterRole(int roleId)
{
    _filterRole = roleId;
    invalidateFilter();
}

void CAIssuerFilter::setIsRootCA(bool isRootCA)
{
    _isRootCA = isRootCA;
    invalidateFilter();
}


bool CAIssuerFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    auto value = sourceModel()->data(index, _filterRole).value<bool>();
    return value == _isRootCA;
}
