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


#include "domaincountlistmodel.h"

domainCountListModel::domainCountListModel(QObject* parent) : GenericListModel<QIntPair>(parent)
{
    addSelector("domain", [](const QIntPair &m) { return m.first; });
    addSelector("count", [](const QIntPair &m) { return m.second; });
}
