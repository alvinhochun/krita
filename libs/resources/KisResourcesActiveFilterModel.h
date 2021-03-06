/*
 * Copyright (C) 2020 Boudewijn Rempt <boud@valdyas.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef KISACTIVEFILTERMODEL_H
#define KISACTIVEFILTERMODEL_H

#include <QSortFilterProxyModel>
#include <KoResource.h>
#include <KisResourceModel.h>

#include "kritaresources_export.h"

/**
 * @brief The KisActiveResourcesModel class
 */
class KRITARESOURCES_EXPORT KisResourceModel : public QSortFilterProxyModel, public KisAbstractResourceModel
{
    Q_OBJECT
public:
    KisResourceModel(int column, QObject *parent);
    ~KisResourceModel() override;

    enum ResourceFilter {
        ShowInactiveResources = 0,
        ShowActiveResources,
        ShowAllResources
    };

    void setResourceFilter(ResourceFilter filter);

    enum StorageFilter {
        ShowInactiveStorages = 0,
        ShowActiveStorages,
        ShowAllStorages
    };

    void setStorageFilter(StorageFilter filter);

public:

    KoResourceSP resourceForIndex(QModelIndex index = QModelIndex()) const override;
    QModelIndex indexFromResource(KoResourceSP resource) const override;
    bool setResourceInactive(const QModelIndex &index) override;
    bool importResourceFile(const QString &filename) override;
    bool addResource(KoResourceSP resource, const QString &storageId = QString()) override;
    bool updateResource(KoResourceSP resource) override;
    bool renameResource(KoResourceSP resource, const QString &name) override;
    bool setResourceInactive(KoResourceSP resource) override;
    bool setResourceMetaData(KoResourceSP resource, QMap<QString, QVariant> metadata) override;

protected:

    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const override;
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private:

    struct Private;
    Private *const d;

    Q_DISABLE_COPY(KisResourcesActiveFilterModel)

};

#endif // KISACTIVEFILTERMODEL_H
