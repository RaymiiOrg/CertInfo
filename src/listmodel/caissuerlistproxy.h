#pragma once

#include "caissuerfilter.h"
#include "caissuerlistmodel.h"

#include <QObject>

class CAIssuerListProxy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(CAIssuerListModel *modelIn READ modelIn WRITE setModelIn NOTIFY modelInChanged)
    Q_PROPERTY(CAIssuerFilter *modelOut READ modelOut CONSTANT)
    Q_PROPERTY(bool isRootCA READ isRootCA WRITE setIsRootCA NOTIFY isRootCAChanged FINAL)

public:
    explicit CAIssuerListProxy(QObject *parent = nullptr);
    void setFilterRole(int roleId) { _filterRole = roleId; }
    CAIssuerListModel *modelIn() const;
    CAIssuerFilter *modelOut() const;

    bool isRootCA() const;
    void setIsRootCA(bool newIsRootCA);

signals:
    void modelInChanged(CAIssuerListModel *modelIn); 
    void isRootCAChanged();

public slots:
    void setModelIn(CAIssuerListModel *modelIn);

private:
    CAIssuerListModel *_modelIn;
    CAIssuerFilter *_modelOut;
    int _filterRole;
    bool m_isRootCA;
};

Q_DECLARE_METATYPE(CAIssuerListProxy *)
