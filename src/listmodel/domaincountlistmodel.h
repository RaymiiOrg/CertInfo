#ifndef DOMAINCOUNTLISTMODEL_H
#define DOMAINCOUNTLISTMODEL_H

#include "genericlistmodel.h"
#include <QObject>

typedef QPair<QString,int> QIntPair;

class domainCountListModel : public GenericListModel<QIntPair>
{
    Q_OBJECT
public:
    domainCountListModel(QObject* parent = nullptr);
};

#endif // DOMAINCOUNTLISTMODEL_H
