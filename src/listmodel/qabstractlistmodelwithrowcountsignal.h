#pragma once

#include <QAbstractListModel>


class QAbstractListModelWithRowCountSignal : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)

public:
    virtual ~QAbstractListModelWithRowCountSignal() = default;

protected:
    QAbstractListModelWithRowCountSignal(QObject *parent = nullptr) :
        QAbstractListModel(parent)
    {
        QObject::connect(this, &QAbstractListModel::rowsInserted, this, &QAbstractListModelWithRowCountSignal::rowCountChanged);
        QObject::connect(this, &QAbstractListModel::rowsRemoved, this, &QAbstractListModelWithRowCountSignal::rowCountChanged);
        QObject::connect(this, &QAbstractListModel::modelReset, this, &QAbstractListModelWithRowCountSignal::rowCountChanged);
        QObject::connect(this, &QAbstractListModel::layoutChanged, this, &QAbstractListModelWithRowCountSignal::rowCountChanged);

        // Rowcount does not change with these, but rowCountChanged is emitted to force updating the visual layout in qml
        QObject::connect(this, &QAbstractListModel::rowsMoved, this, &QAbstractListModelWithRowCountSignal::rowCountChanged);
        QObject::connect(this, &QAbstractListModel::dataChanged, this, &QAbstractListModelWithRowCountSignal::rowCountChanged);
    }

signals:
    void rowCountChanged();
};
