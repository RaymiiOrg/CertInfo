#pragma once

#include "qsortfilterproxymodelwithrowcountsignal.h"

class CAIssuerFilter : public QSortFilterProxyModelWithRowCountSignal
{
    Q_OBJECT
public:
    CAIssuerFilter(QObject *parent = nullptr);
    void setFilterRole(int roleId);
    void setIsRootCA(bool isRootCA);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    int _filterRole;
    bool _isRootCA;

};

Q_DECLARE_METATYPE(CAIssuerFilter *)
