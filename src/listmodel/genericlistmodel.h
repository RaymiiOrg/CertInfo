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

#include "qabstractlistmodelwithrowcountsignal.h"

template <typename TObject>
class GenericListModel : public QAbstractListModelWithRowCountSignal
{
    // Q_OBJECT macro is not supported in template classes.
    // This is not a problem, but qmlRegisterType is not possible with E.G. GenericListModel<MyQObject>.
    // A derived class MyQObjectListModel : public GenericListModel<MyQobject> is still necessary.
public:
    GenericListModel(QObject *parent = nullptr) :
        QAbstractListModelWithRowCountSignal(parent) { }

    void clear();
    void updateFromVector(const std::vector<TObject> &newObjects);
    void updateFromQList(const QList<TObject> &newObjects);
    void addRow(const TObject&);
    void updateRow(int rowNr, const TObject&);
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual void move(int from, int to);
    int addSelector(QByteArray name, std::function<QVariant(const TObject &, const QModelIndex &)> selector);
    int addSelector(QByteArray name, std::function<QVariant(const TObject &)> selector);
    int addSelector(QByteArray name, std::function<QVariant(const TObject &)> selector, std::function<bool(TObject &, QVariant)> valueSetter);
    int addSelector(QByteArray name, std::function<QVariant(const TObject &, const QModelIndex &)> selector, std::function<bool(TObject &, QVariant)> valueSetter);
    QVariant dataByRoleName(const QModelIndex &index, const QString &name) const;
    void resync();
    int getRoleIdByName(const QString &name) const;
    QVariantMap getRowByField(const QString &fieldName, const QVariant &value) const;

protected:
    QList<TObject> m_listObjects;
    QMap<int, QPair<QByteArray, std::function<QVariant(const TObject &, const QModelIndex &)>>> m_selectors;
    QMap<int, QPair<QByteArray, std::function<bool(TObject &, QVariant)>>> m_valueSetters;
};

template <typename TObject>
void GenericListModel<TObject>::updateFromVector(const std::vector<TObject> &newObjects)
{
    beginResetModel();
    m_listObjects.clear();
    for (const TObject &item : newObjects)
    {
        m_listObjects << item;
    }
    endResetModel();
}


template <typename TObject>
void GenericListModel<TObject>::updateFromQList(const QList<TObject> &newObjects)
{
    beginResetModel();
    m_listObjects = newObjects;
    endResetModel();
}

template <typename TObject>
void GenericListModel<TObject>::clear()
{
    beginResetModel();
    m_listObjects.clear();
    endResetModel();
}

template <typename TObject>
QHash<int, QByteArray> GenericListModel<TObject>::roleNames() const
{
    QHash<int, QByteArray> roles;
    for (auto it = m_selectors.begin(); it != m_selectors.end(); ++it)
    {
        roles[it.key()] = it->first;
    }
    return roles;
}

template <typename TObject>
QVariant GenericListModel<TObject>::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    const TObject &object = m_listObjects[index.row()];
    auto selectorIterator = m_selectors.find(role);
    if (selectorIterator != m_selectors.end())
        return selectorIterator->second(object, index);
    return QVariant();
}

template <typename TObject>
bool GenericListModel<TObject>::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    bool result = false;
    TObject &object = m_listObjects[index.row()];
    auto valueSetterIterator = m_valueSetters.find(role);
    if (valueSetterIterator != m_valueSetters.end())
    {
        result = valueSetterIterator->second(object, value);
        if (result)
            emit dataChanged(index, index, {role});
    }
    return result;
}

template <typename TObject>
int GenericListModel<TObject>::rowCount(const QModelIndex &index) const
{
    if (index.isValid())
    {
        return 0;
    }
    return m_listObjects.count();
}

template <typename TObject>
void GenericListModel<TObject>::move(int from, int to)
{
    m_listObjects.move(from, to);

}

template <typename TObject>
int GenericListModel<TObject>::addSelector(QByteArray name, std::function<QVariant(const TObject &)> selector)
{
    std::function<QVariant(TObject, const QModelIndex &)> newselector = std::bind(selector, std::placeholders::_1);
    return addSelector(name, newselector);
}

template <typename TObject>
int GenericListModel<TObject>::addSelector(QByteArray name, std::function<QVariant(const TObject &)> selector, std::function<bool(TObject &, QVariant)> valueSetter)
{
    int roleNumber = addSelector(name, selector);
    m_valueSetters[roleNumber] = QPair<QByteArray, std::function<bool(TObject &, QVariant)>> {name, valueSetter};
    return roleNumber;
}

template <typename TObject>
int GenericListModel<TObject>::addSelector(QByteArray name, std::function<QVariant(const TObject &, const QModelIndex &)> selector, std::function<bool(TObject &, QVariant)> valueSetter)
{
    int roleNumber = addSelector(name, selector);
    m_valueSetters[roleNumber] = QPair<QByteArray, std::function<bool(TObject &, QVariant)>> {name, valueSetter};
    return roleNumber;
}

template <typename TObject>
int GenericListModel<TObject>::addSelector(QByteArray name, std::function<QVariant(const TObject &, const QModelIndex &)> selector)
{
    int roleNumber = Qt::UserRole + m_selectors.size() + 1;
    m_selectors[roleNumber] = QPair<QByteArray, std::function<QVariant(TObject, const QModelIndex &)>> {name, selector};
    return roleNumber;
}

template <typename TObject>
QVariant GenericListModel<TObject>::dataByRoleName(const QModelIndex &index, const QString &roleName) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    const TObject &object = m_listObjects[index.row()];
    for (const QPair<QByteArray, std::function<QVariant(const TObject &, const QModelIndex &)>> &mapEntry : m_selectors)
    {
        if (mapEntry.first == roleName)
            return mapEntry.second(object, index);
    }
    return QVariant();
}

template <typename TObject>
void GenericListModel<TObject>::resync()
{
    std::vector<TObject> copiedListObjects;
    std::copy(m_listObjects.begin(), m_listObjects.end(), std::back_inserter(copiedListObjects));
    updateFromVector(copiedListObjects);
}


template <typename TObject>
void GenericListModel<TObject>::addRow(const TObject& newRow)
{
    auto size = m_listObjects.size();
    emit beginInsertRows(QModelIndex(), size, size);
    m_listObjects.push_back(newRow);
    emit endInsertRows();
}


template <typename TObject>
void GenericListModel<TObject>::updateRow(int rowNr, const TObject& newRow)
{
    if (rowNr >= m_listObjects.size())
        return;
    m_listObjects[rowNr] = newRow;
    emit dataChanged(index(rowNr), index(rowNr));
}


template <typename TObject>
int GenericListModel<TObject>::getRoleIdByName(const QString &name) const
{
    QHash<int, QByteArray> names = roleNames();
    auto iter = std::find_if(names.begin(), names.end(), [name](QByteArray roleName) { return roleName == name; });
    return iter == names.end() ? -1 : iter.key();
}

template <typename TObject>
QVariantMap GenericListModel<TObject>::getRowByField(const QString &fieldName, const QVariant &value) const
{
    QVariantMap data;
    QModelIndexList matches = this->match(this->index(0, 0), getRoleIdByName(fieldName), value, 1, Qt::MatchExactly);

    if (matches.begin() == matches.end())
        return data;

    const QModelIndex idx = *matches.begin();

    if (!idx.isValid())
        return data;

    const QHash<int, QByteArray> rn = this->roleNames();

    for (auto it = rn.begin(); it != rn.end(); ++it)
        data[it.value()] = idx.data(it.key());

    return data;
}
