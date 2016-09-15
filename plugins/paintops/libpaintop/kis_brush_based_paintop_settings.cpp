/*
 *  Copyright (c) 2010 Sven Langkamp <sven.langkamp@gmail.com>
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

#include "kis_brush_based_paintop_settings.h"

#include <kis_paint_action_type_option.h>
#include <kis_airbrush_option.h>
#include "kis_brush_based_paintop_options_widget.h"
#include <kis_boundary.h>
#include "kis_brush_server.h"
#include <QLineF>
#include "kis_signals_blocker.h"
#include "kis_brush_option.h"

struct BrushReader {
    BrushReader(const KisBrushBasedPaintOpSettings *parent)
        : m_parent(parent)
    {
        m_option.readOptionSetting(m_parent, false);
    }

    KisBrushSP brush() {
        return m_option.brush();
    }

    const KisBrushBasedPaintOpSettings *m_parent;
    KisBrushOption m_option;
};

struct BrushWriter {
    BrushWriter(KisBrushBasedPaintOpSettings *parent)
        : m_parent(parent)
    {
        m_option.readOptionSetting(m_parent, false);
    }

    ~BrushWriter() {
        m_option.writeOptionSetting(m_parent);
    }

    KisBrushSP brush() {
        return m_option.brush();
    }

    KisBrushBasedPaintOpSettings *m_parent;
    KisBrushOption m_option;
};


KisBrushBasedPaintOpSettings::KisBrushBasedPaintOpSettings()
    : KisOutlineGenerationPolicy<KisPaintOpSettings>(KisCurrentOutlineFetcher::SIZE_OPTION |
            KisCurrentOutlineFetcher::ROTATION_OPTION |
            KisCurrentOutlineFetcher::MIRROR_OPTION)
{
}

bool KisBrushBasedPaintOpSettings::paintIncremental()
{
    if (hasProperty("PaintOpAction")) {
        return (enumPaintActionType)getInt("PaintOpAction", WASH) == BUILDUP;
    }
    return true;
}

bool KisBrushBasedPaintOpSettings::isAirbrushing() const
{
    return getBool(AIRBRUSH_ENABLED);
}


int KisBrushBasedPaintOpSettings::rate() const
{
    return getInt(AIRBRUSH_RATE);
}

KisPaintOpSettingsSP KisBrushBasedPaintOpSettings::clone() const
{
    KisPaintOpSettingsSP _settings = KisOutlineGenerationPolicy<KisPaintOpSettings>::clone();
    KisBrushBasedPaintOpSettingsSP settings = dynamic_cast<KisBrushBasedPaintOpSettings*>(_settings.data());
    settings->m_savedBrush = this->brush();
    return settings;
}

KisBrushSP KisBrushBasedPaintOpSettings::brush() const
{
    BrushReader w(this);
    if (!w.brush()) return 0;

    return w.brush();
}

QPainterPath KisBrushBasedPaintOpSettings::brushOutlineImpl(const KisPaintInformation &info,
                                                            OutlineMode mode,
                                                            qreal additionalScale,
                                                            bool forceOutline)
{
    QPainterPath path;

    if (forceOutline || mode == CursorIsOutline || mode == CursorIsCircleOutline || mode == CursorTiltOutline) {
        KisBrushSP brush = this->brush();
        if (!brush) return path;
        qreal finalScale = brush->scale() * additionalScale;

        QPainterPath realOutline = brush->outline();

        if (mode == CursorIsCircleOutline || mode == CursorTiltOutline ||
            (forceOutline && mode == CursorNoOutline)) {

            QPainterPath ellipse;
            ellipse.addEllipse(realOutline.boundingRect());
            realOutline = ellipse;
        }

        path = outlineFetcher()->fetchOutline(info, this, realOutline, finalScale, brush->angle());

        if (mode == CursorTiltOutline) {
            QPainterPath tiltLine = makeTiltIndicator(info,
                realOutline.boundingRect().center(),
                realOutline.boundingRect().width() * 0.5,
                3.0);
            path.addPath(outlineFetcher()->fetchOutline(info, this, tiltLine, finalScale, 0.0, true, realOutline.boundingRect().center().x(), realOutline.boundingRect().center().y()));
        }
    }

    return path;
}

QPainterPath KisBrushBasedPaintOpSettings::brushOutline(const KisPaintInformation &info, OutlineMode mode)
{
    return brushOutlineImpl(info, mode, 1.0);
}

bool KisBrushBasedPaintOpSettings::isValid() const
{
    QString filename = getString("requiredBrushFile", "");
    if (!filename.isEmpty()) {
        KisBrushSP brush = KisBrushServer::instance()->brushServer()->resourceByFilename(filename);
        if (!brush) {
            return false;
        }
    }
    return true;
}

bool KisBrushBasedPaintOpSettings::isLoadable()
{
    return (KisBrushServer::instance()->brushServer()->resources().count() > 0);
}

void KisBrushBasedPaintOpSettings::setAngle(qreal value)
{
    BrushWriter w(this);
    if (!w.brush()) return;
    w.brush()->setAngle(value);
}

qreal KisBrushBasedPaintOpSettings::angle()
{
    BrushReader w(this);
    if (!w.brush()) return 0.0;
    return w.brush()->angle();
}

void KisBrushBasedPaintOpSettings::setSpacing(qreal value)
{
    BrushWriter w(this);
    if (!w.brush()) return;
    w.brush()->setSpacing(value);
}

qreal KisBrushBasedPaintOpSettings::spacing()
{
    BrushReader w(this);
    if (!w.brush()) return 0.0;
    return w.brush()->spacing();
}

void KisBrushBasedPaintOpSettings::setAutoSpacing(bool active, qreal coeff)
{
    BrushWriter w(this);
    if (!w.brush()) return;
    w.brush()->setAutoSpacing(active, coeff);
}


bool KisBrushBasedPaintOpSettings::autoSpacingActive()
{
    BrushReader w(this);
    if (!w.brush()) return 0.0;
    return w.brush()->autoSpacingActive();
}

qreal KisBrushBasedPaintOpSettings::autoSpacingCoeff()
{
    BrushReader w(this);
    if (!w.brush()) return 0.0;
    return w.brush()->autoSpacingCoeff();
}

void KisBrushBasedPaintOpSettings::setPaintOpSize(qreal value)
{
    BrushWriter w(this);
    if (!w.brush()) return;

    w.brush()->setUserEffectiveSize(value);
}

qreal KisBrushBasedPaintOpSettings::paintOpSize() const
{
    BrushReader w(this);
    if (!w.brush()) return 0.0;

    return w.brush()->userEffectiveSize();
}



#include <brushengine/kis_slider_based_paintop_property.h>
#include "kis_paintop_preset.h"
#include "kis_paintop_settings_update_proxy.h"

QList<KisUniformPaintOpPropertySP> KisBrushBasedPaintOpSettings::uniformProperties()
{
    QList<KisUniformPaintOpPropertySP> props =
        listWeakToStrong(m_uniformProperties);

    if (props.isEmpty()) {
        {
            KisIntSliderBasedPaintOpPropertyCallback *prop =
                new KisIntSliderBasedPaintOpPropertyCallback(
                    KisIntSliderBasedPaintOpPropertyCallback::Int,
                    "angle",
                    "Angle",
                    this, 0);

            prop->setRange(0, 360);

            prop->setReadCallback(
                [](KisUniformPaintOpProperty *prop) {
                    KisBrushBasedPaintOpSettings *s =
                        dynamic_cast<KisBrushBasedPaintOpSettings*>(prop->settings().data());

                    const qreal angleResult = kisRadiansToDegrees(s->angle());
                    prop->setValue(angleResult);
                });
            prop->setWriteCallback(
                [](KisUniformPaintOpProperty *prop) {
                    KisBrushBasedPaintOpSettings *s =
                        dynamic_cast<KisBrushBasedPaintOpSettings*>(prop->settings().data());

                    s->setAngle(kisDegreesToRadians(prop->value().toReal()));
                });

            QObject::connect(preset()->updateProxy(), SIGNAL(sigSettingsChanged()), prop, SLOT(requestReadValue()));
            prop->requestReadValue();
            props << toQShared(prop);
        }
        {
            KisUniformPaintOpPropertyCallback *prop =
                new KisUniformPaintOpPropertyCallback(
                    KisUniformPaintOpPropertyCallback::Bool,
                    "auto_spacing",
                    "Auto Spacing",
                    this, 0);

            prop->setReadCallback(
                [](KisUniformPaintOpProperty *prop) {
                    KisBrushBasedPaintOpSettings *s =
                        dynamic_cast<KisBrushBasedPaintOpSettings*>(prop->settings().data());

                    prop->setValue(s->autoSpacingActive());
                });
            prop->setWriteCallback(
                [](KisUniformPaintOpProperty *prop) {
                    KisBrushBasedPaintOpSettings *s =
                        dynamic_cast<KisBrushBasedPaintOpSettings*>(prop->settings().data());

                    s->setAutoSpacing(prop->value().toBool(), s->autoSpacingCoeff());
                });

            QObject::connect(preset()->updateProxy(), SIGNAL(sigSettingsChanged()), prop, SLOT(requestReadValue()));
            prop->requestReadValue();
            props << toQShared(prop);
        }

        {
            KisDoubleSliderBasedPaintOpPropertyCallback *prop =
                new KisDoubleSliderBasedPaintOpPropertyCallback(
                    KisDoubleSliderBasedPaintOpPropertyCallback::Double,
                    "spacing",
                    "Spacing",
                    this, 0);

            prop->setRange(0.01, 10);
            prop->setSingleStep(0.01);

            prop->setReadCallback(
                [](KisUniformPaintOpProperty *prop) {
                    KisBrushBasedPaintOpSettings *s =
                        dynamic_cast<KisBrushBasedPaintOpSettings*>(prop->settings().data());

                    const qreal value = s->autoSpacingActive() ?
                        s->autoSpacingCoeff() : s->spacing();
                    prop->setValue(value);
                });
            prop->setWriteCallback(
                [](KisUniformPaintOpProperty *prop) {
                    KisBrushBasedPaintOpSettings *s =
                        dynamic_cast<KisBrushBasedPaintOpSettings*>(prop->settings().data());

                    if (s->autoSpacingActive()) {
                        s->setAutoSpacing(true, prop->value().toReal());
                    } else {
                        s->setSpacing(prop->value().toReal());
                    }
                });

            QObject::connect(preset()->updateProxy(), SIGNAL(sigSettingsChanged()), prop, SLOT(requestReadValue()));
            prop->requestReadValue();
            props << toQShared(prop);
        }
    }

    return KisPaintOpSettings::uniformProperties() + props;
}
