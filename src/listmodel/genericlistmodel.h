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
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual void move(int from, int to);
    int addValueSelector(QByteArray name, std::function<QVariant(const TObject &, const QModelIndex &)> valueSelector);
    int addValueSelector(QByteArray name, std::function<QVariant(const TObject &)> valueSelector);
    int addValueSelector(QByteArray name, std::function<QVariant(const TObject &)> valueSelector, std::function<bool(TObject &, QVariant)> valueSetter);
    int addValueSelector(QByteArray name, std::function<QVariant(const TObject &, const QModelIndex &)> valueSelector, std::function<bool(TObject &, QVariant)> valueSetter);

    QVariant dataByRoleName(const QModelIndex &index, const QString &name) const;

    void resync();
    void updateRow(int rowNr, TObject);

    int getRoleIdByName(const QString &name) const;
    QVariantMap getRowById(const QString &rowId) const;
    QVariantMap getRowByField(const QString &fieldName, const QVariant &value) const;
    QModelIndex getIndexById(const QString &rowId) const;

protected:
    QList<TObject> _listObjects;
    QMap<int, QPair<QByteArray, std::function<QVariant(const TObject &, const QModelIndex &)>>> _valueSelectorMap;
    QMap<int, QPair<QByteArray, std::function<bool(TObject &, QVariant)>>> _valueSetterMap;

private:
    // This method is called before the internal data structure is replaced/updated.
    virtual void beforeModelUpdatedFromVector(const std::vector<TObject> &newObjects)
    {
        Q_UNUSED(newObjects);
    }

    // This method is called after the internal data structure is replaced/updated.
    virtual void afterModelUpdatedFromVector(const std::vector<TObject> &newObjects)
    {
        Q_UNUSED(newObjects);
    }
};

template <typename TObject>
void GenericListModel<TObject>::updateFromVector(const std::vector<TObject> &newObjects)
{
    beginResetModel();
    // Give derived classes the opportunity to do stuff before data is updated (e.g. save some state based on old data)
    beforeModelUpdatedFromVector(newObjects);
    _listObjects.clear();
    for (const TObject &item : newObjects)
    {
        _listObjects << item;
    }
    // Give derived classes the opportunity to do stuff after data is updated (e.g. restore state or (re)build extra internal structures)
    afterModelUpdatedFromVector(newObjects);
    endResetModel();
}


template <typename TObject>
void GenericListModel<TObject>::clear()
{
    beginResetModel();
    _listObjects.clear();
    endResetModel();
}

template <typename TObject>
QHash<int, QByteArray> GenericListModel<TObject>::roleNames() const
{
    QHash<int, QByteArray> roles;
    for (auto it = _valueSelectorMap.begin(); it != _valueSelectorMap.end(); ++it)
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

    const TObject &object = _listObjects[index.row()];
    auto valueSelectorIterator = _valueSelectorMap.find(role);
    if (valueSelectorIterator != _valueSelectorMap.end())
        return valueSelectorIterator->second(object, index);
    return QVariant();
}

template <typename TObject>
bool GenericListModel<TObject>::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    bool result = false;
    TObject &object = _listObjects[index.row()];
    auto valueSetterIterator = _valueSetterMap.find(role);
    if (valueSetterIterator != _valueSetterMap.end())
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
    return _listObjects.count();
}

template <typename TObject>
void GenericListModel<TObject>::move(int from, int to)
{
    beginResetModel();
    _listObjects.move(from, to);
    endResetModel();
}

template <typename TObject>
int GenericListModel<TObject>::addValueSelector(QByteArray name, std::function<QVariant(const TObject &)> valueSelector)
{
    // std::bind ignores extra arguments, (I.E. QModelIndex)
    std::function<QVariant(TObject, const QModelIndex &)> newValueSelector = std::bind(valueSelector, std::placeholders::_1);
    return addValueSelector(name, newValueSelector);
}

template <typename TObject>
int GenericListModel<TObject>::addValueSelector(QByteArray name, std::function<QVariant(const TObject &)> valueSelector, std::function<bool(TObject &, QVariant)> valueSetter)
{
    int roleNumber = addValueSelector(name, valueSelector);
    _valueSetterMap[roleNumber] = QPair<QByteArray, std::function<bool(TObject &, QVariant)>> {name, valueSetter};
    return roleNumber;
}

template <typename TObject>
int GenericListModel<TObject>::addValueSelector(QByteArray name, std::function<QVariant(const TObject &, const QModelIndex &)> valueSelector, std::function<bool(TObject &, QVariant)> valueSetter)
{
    int roleNumber = addValueSelector(name, valueSelector);
    _valueSetterMap[roleNumber] = QPair<QByteArray, std::function<bool(TObject &, QVariant)>> {name, valueSetter};
    return roleNumber;
}

template <typename TObject>
int GenericListModel<TObject>::addValueSelector(QByteArray name, std::function<QVariant(const TObject &, const QModelIndex &)> valueSelector)
{
    int roleNumber = Qt::UserRole + _valueSelectorMap.size() + 1;
    _valueSelectorMap[roleNumber] = QPair<QByteArray, std::function<QVariant(TObject, const QModelIndex &)>> {name, valueSelector};
    return roleNumber;
}

template <typename TObject>
QVariant GenericListModel<TObject>::dataByRoleName(const QModelIndex &index, const QString &roleName) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    const TObject &object = _listObjects[index.row()];
    for (const QPair<QByteArray, std::function<QVariant(const TObject &, const QModelIndex &)>> &mapEntry : _valueSelectorMap)
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
    std::copy(_listObjects.begin(), _listObjects.end(), std::back_inserter(copiedListObjects));
    updateFromVector(copiedListObjects);
}

template <typename TObject>
void GenericListModel<TObject>::updateRow(int rowNr, TObject newRow)
{
    if (rowNr >= _listObjects.size())
        return;
    _listObjects[rowNr] = newRow;
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
QVariantMap GenericListModel<TObject>::getRowById(const QString &rowId) const
{
    return getRowByField("id", rowId);
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

template <typename TObject>
QModelIndex GenericListModel<TObject>::getIndexById(const QString &rowId) const
{
    QModelIndexList matches = this->match(this->index(0, 0), this->getRoleIdByName("id"), rowId, 1, Qt::MatchExactly);

    if (matches.begin() == matches.end())
        return QModelIndex();

    return *matches.begin();
}
