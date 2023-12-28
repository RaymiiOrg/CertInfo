#pragma once

#include <QSortFilterProxyModel>

class QSortFilterProxyModelWithRowCountSignal : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)

public:
    virtual ~QSortFilterProxyModelWithRowCountSignal() = default;

protected:
    QSortFilterProxyModelWithRowCountSignal(QObject *parent = nullptr) :
        QSortFilterProxyModel(parent)
    {
        QObject::connect(this, &QSortFilterProxyModel::rowsInserted, this, &QSortFilterProxyModelWithRowCountSignal::rowCountChanged);
        QObject::connect(this, &QSortFilterProxyModel::rowsRemoved, this, &QSortFilterProxyModelWithRowCountSignal::rowCountChanged);
        QObject::connect(this, &QSortFilterProxyModel::modelReset, this, &QSortFilterProxyModelWithRowCountSignal::rowCountChanged);
        QObject::connect(this, &QSortFilterProxyModel::layoutChanged, this, &QSortFilterProxyModelWithRowCountSignal::rowCountChanged);

        // Rowcount does not change with these, but rowCountChanged is emitted to force updating the visual layout in qml
        QObject::connect(this, &QSortFilterProxyModel::rowsMoved, this, &QSortFilterProxyModelWithRowCountSignal::rowCountChanged);
        QObject::connect(this, &QSortFilterProxyModel::dataChanged, this, &QSortFilterProxyModelWithRowCountSignal::rowCountChanged);
    }

signals:
    void rowCountChanged();
};
