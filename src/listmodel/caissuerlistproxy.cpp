#include "CAIssuerListProxy.h"
#include <QSortFilterProxyModel>

CAIssuerListProxy::CAIssuerListProxy(QObject *parent) :
    QObject(parent),
    _modelOut(new CAIssuerFilter(this))
{
}

CAIssuerListModel *CAIssuerListProxy::modelIn() const
{
    return _modelIn;
}

CAIssuerFilter *CAIssuerListProxy::modelOut() const
{
    return _modelOut;
}

bool CAIssuerListProxy::isRootCA() const
{
    return m_isRootCA;
}

void CAIssuerListProxy::setIsRootCA(bool isRootCA)
{
    if (m_isRootCA == isRootCA)
        return;

    m_isRootCA = isRootCA;
    _modelOut->setIsRootCA(m_isRootCA);

    _modelOut->invalidate();

    emit isRootCAChanged();
}

void CAIssuerListProxy::setModelIn(CAIssuerListModel *modelIn)
{
    if (_modelIn == modelIn)
        return;

    _modelIn = modelIn;
    _modelOut->setSourceModel(_modelIn);
    _modelOut->setFilterRole(_modelIn->getRoleIdByName("isrootca"));

    _modelOut->invalidate();
    emit modelInChanged(_modelIn);
}



