#ifndef CAISSUERLISTMODEL_H
#define CAISSUERLISTMODEL_H

#include "src/ca/certificate.h"
#include "genericlistmodel.h"
#include <QPair>
#include <QObject>

class CACertificateListModel : public GenericListModel<Certificate>
{
    Q_OBJECT
public:
    CACertificateListModel(QObject* parent = nullptr);
};

#endif // CAISSUERLISTMODEL_H
