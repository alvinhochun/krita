/*
 * This file is part of Krita
 *
 * Copyright (c) 2005 Michael Thaler <michael.thaler@physik.tu-muenchen.de>
 *
 * ported from Gimp, Copyright (C) 1997 Eiichi Takamori <taka@ma1.seikyou.ne.jp>
 * original pixelize.c for GIMP 0.54 by Tracy Scott
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_pixelize_filter.h"


#include <stdlib.h>
#include <vector>

#include <QPoint>
#include <QSpinBox>
#include <QVector>

#include <klocalizedstring.h>
#include <kpluginfactory.h>

#include <KoUpdater.h>

#include <kis_debug.h>
#include <KisDocument.h>
#include <filter/kis_filter_registry.h>
#include <kis_global.h>
#include <kis_image.h>
#include <kis_layer.h>
#include <kis_selection.h>
#include <kis_types.h>
#include <filter/kis_filter_category_ids.h>
#include <filter/kis_filter_configuration.h>
#include <kis_processing_information.h>

#include "widgets/kis_multi_integer_filter_widget.h"
#include <KoMixColorsOp.h>
#include <KisSequentialIteratorProgress.h>
#include "kis_algebra_2d.h"
#include "kis_lod_transform.h"

#ifdef USE_RUST
#include "krita_filter_pixelize_rs.hpp"
extern "C" {

void ffi_KisSequentialConstIterator__ctor_checked(KisSequentialConstIterator *this_ptr,
                                                  size_t this_size,
                                                  KisPaintDeviceSP &src_dev,
                                                  qint32 left,
                                                  qint32 top,
                                                  qint32 width,
                                                  qint32 height)
{
    if (this_size < sizeof(KisSequentialConstIterator)) {
        qFatal("ffi_KisSequentialConstIterator__ctor_checked: this_size is %zd but sizeof(KisSequentialConstIterator) is %zd",
               this_size,
               sizeof(KisSequentialConstIterator));
        std::abort();
    }
    new (this_ptr) KisSequentialConstIterator(src_dev, QRect(left, top, width, height));
}

void ffi_KisSequentialConstIterator__dtor(KisSequentialConstIterator *this_ptr)
{
    this_ptr->~KisSequentialConstIterator();
}

bool ffi_KisSequentialConstIterator_nextPixel(KisSequentialConstIterator *it)
{
    return it->nextPixel();
}

const quint8 *ffi_KisSequentialConstIterator_oldRawData(const KisSequentialConstIterator *it)
{
    return it->oldRawData();
}

void ffi_KisSequentialIterator__ctor_checked(KisSequentialIterator *this_ptr,
                                                  size_t this_size,
                                                  const KisPaintDeviceSP &src_dev,
                                                  qint32 left,
                                                  qint32 top,
                                                  qint32 width,
                                                  qint32 height)
{
    if (this_size < sizeof(KisSequentialIterator)) {
        qFatal("ffi_KisSequentialIterator__ctor_checked: this_size is %zd but sizeof(KisSequentialIterator) is %zd",
               this_size,
               sizeof(KisSequentialIterator));
        std::abort();
    }
    new (this_ptr) KisSequentialIterator(src_dev, QRect(left, top, width, height));
}

void ffi_KisSequentialIterator__dtor(KisSequentialIterator *this_ptr)
{
    this_ptr->~KisSequentialIterator();
}

bool ffi_KisSequentialIterator_nextPixel(KisSequentialIterator *it)
{
    return it->nextPixel();
}

quint8 *ffi_KisSequentialIterator_rawData(const KisSequentialIterator *it)
{
    // KisSequentialIterator::rawData doesn't actually mutate the iterator itself, but it isn't declared const.
    return const_cast<KisSequentialIterator *>(it)->rawData();
}

void koMixColorsOpMixColors(const KoMixColorsOp *mixOp, const quint8 *colors, quint32 nColors, quint8 *dst)
{
    mixOp->mixColors(colors, nColors, dst);
}

} // extern "C"
#endif

KisPixelizeFilter::KisPixelizeFilter() : KisFilter(id(), FiltersCategoryArtisticId, i18n("&Pixelize..."))
{
    setSupportsPainting(true);
    setSupportsThreading(true);
    setSupportsAdjustmentLayers(true);
    setSupportsLevelOfDetail(true);
    setColorSpaceIndependence(FULLY_INDEPENDENT);
}

void KisPixelizeFilter::processImpl(KisPaintDeviceSP device,
                                    const QRect& applyRect,
                                    const KisFilterConfigurationSP config,
                                    KoUpdater* progressUpdater
                                    ) const
{
    Q_ASSERT(device);

    KisLodTransformScalar t(device);
    const int pixelWidth = qCeil(t.scale(config ? qMax(1, config->getInt("pixelWidth", 10)) : 10));
    const int pixelHeight = qCeil(t.scale(config ? qMax(1, config->getInt("pixelHeight", 10)) : 10));

    const qint32 pixelSize = device->pixelSize();

    const QRect deviceBounds = device->defaultBounds()->bounds();

    const int bufferSize = pixelSize * pixelWidth * pixelHeight;
    QScopedArrayPointer<quint8> buffer(new quint8[bufferSize]);

    KoColor pixelColor(Qt::black, device->colorSpace());
    KoMixColorsOp *mixOp = device->colorSpace()->mixColorsOp();

    using namespace KisAlgebra2D;
    const qint32 firstCol = divideFloor(applyRect.x(), pixelWidth);
    const qint32 firstRow = divideFloor(applyRect.y(), pixelHeight);

    const qint32 lastCol = divideFloor(applyRect.x() + applyRect.width() - 1, pixelWidth);
    const qint32 lastRow = divideFloor(applyRect.y() + applyRect.height() - 1, pixelHeight);

    progressUpdater->setRange(firstRow, lastRow);

    for(qint32 i = firstRow; i <= lastRow; i++) {
        for(qint32 j = firstCol; j <= lastCol; j++) {
            const QRect maxPatchRect(j * pixelWidth, i * pixelHeight,
                                     pixelWidth, pixelHeight);
            const QRect pixelRect = maxPatchRect & deviceBounds;
            const int numColors = pixelRect.width() * pixelRect.height();


            // write only colors in applyRect
            const QRect writeRect = pixelRect & applyRect;

            memset(buffer.data(), 0, bufferSize);
            quint8 *bufferPtr = buffer.data();

#ifdef USE_RUST
            krita_filter_pixelize_rs_process_block(&device,
                                                   pixelRect.left(),
                                                   pixelRect.top(),
                                                   pixelRect.width(),
                                                   pixelRect.height(),
                                                   writeRect.left(),
                                                   writeRect.top(),
                                                   writeRect.width(),
                                                   writeRect.height(),
                                                   pixelSize,
                                                   pixelWidth,
                                                   pixelHeight,
                                                   mixOp,
                                                   bufferPtr,
                                                   numColors,
                                                   pixelColor.data());
#else
            //read
            KisSequentialConstIterator srcIt(device, pixelRect);
            while (srcIt.nextPixel()) {
                memcpy(bufferPtr, srcIt.oldRawData(), pixelSize);
                bufferPtr += pixelSize;
            }

            // mix all the colors
            mixOp->mixColors(buffer.data(), numColors, pixelColor.data());

            KisSequentialIterator dstIt(device, writeRect);
            while (dstIt.nextPixel()) {
                memcpy(dstIt.rawData(), pixelColor.data(), pixelSize);
            }
#endif
        }
        progressUpdater->setValue(i);
    }
}

QRect KisPixelizeFilter::neededRect(const QRect &rect, const KisFilterConfigurationSP config, int lod) const
{
    KisLodTransformScalar t(lod);

    const int pixelWidth = qCeil(t.scale(config ? qMax(1, config->getInt("pixelWidth", 10)) : 10));
    const int pixelHeight = qCeil(t.scale(config ? qMax(1, config->getInt("pixelHeight", 10)) : 10));

    // TODO: make more precise calculation of the rect, including the alignment
    return rect.adjusted(-2*pixelWidth, -2*pixelHeight, 2*pixelWidth, 2*pixelHeight);
}

QRect KisPixelizeFilter::changedRect(const QRect &rect, const KisFilterConfigurationSP config, int lod) const
{
    return neededRect(rect, config, lod);
}

KisConfigWidget * KisPixelizeFilter::createConfigurationWidget(QWidget* parent, const KisPaintDeviceSP) const
{
    vKisIntegerWidgetParam param;
    param.push_back(KisIntegerWidgetParam(2, 512, 10, i18n("Pixel width"), "pixelWidth"));
    param.push_back(KisIntegerWidgetParam(2, 512, 10, i18n("Pixel height"), "pixelHeight"));
    return new KisMultiIntegerFilterWidget(id().id(),  parent,  id().id(),  param);
}

KisFilterConfigurationSP KisPixelizeFilter::factoryConfiguration() const
{
    KisFilterConfigurationSP config = new KisFilterConfiguration("pixelize", 1);
    config->setProperty("pixelWidth", 10);
    config->setProperty("pixelHeight", 10);
    return config;
}

