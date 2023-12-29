/*
 * Copyright (c) 2023 Remy van Elst https://raymii.org
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
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
        QObject::connect(this, &QAbstractListModel::rowsMoved, this, &QAbstractListModelWithRowCountSignal::rowCountChanged);
        QObject::connect(this, &QAbstractListModel::dataChanged, this, &QAbstractListModelWithRowCountSignal::rowCountChanged);
    }

signals:
    void rowCountChanged();
};
