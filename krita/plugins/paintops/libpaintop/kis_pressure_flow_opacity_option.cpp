/*
 * Copyright (c) 2011 Silvio Heinrich <plassy@web.de>
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

#include "kis_pressure_flow_opacity_option.h"
#include "kis_paint_action_type_option.h"

#include <klocale.h>

#include <kis_painter.h>
#include <kis_paint_information.h>
#include <kis_indirect_painting_support.h>
#include <kis_node.h>
#include <widgets/kis_curve_widget.h>

KisPixelBrushOpacityOption::KisPixelBrushOpacityOption(KisNodeSP currentNode)
    : KisCurveOption(i18n("Opacity"), "Opacity", KisPaintOpOption::generalCategory(), true, 1.0, 0.0, 1.0)
{
    setCurveUsed(true);
    setSeparateCurveValue(true);

    m_checkable = false;
    setMinimumLabel(i18n("Transparent"));
    setMaximumLabel(i18n("Opaque"));

    m_nodeHasIndirectPaintingSupport =
        currentNode &&
        dynamic_cast<KisIndirectPaintingSupport*>(currentNode.data());
}

void KisPixelBrushOpacityOption::writeOptionSetting(KisPropertiesConfiguration* setting) const
{
    KisCurveOption::writeOptionSetting(setting);
}

void KisPixelBrushOpacityOption::readOptionSetting(const KisPropertiesConfiguration* setting)
{
    KisCurveOption::readOptionSetting(setting);
    m_paintActionType = setting->getInt("PaintOpAction", BUILDUP);
}


qreal KisPixelBrushOpacityOption::getStaticOpacity() const
{
    return value();
}

qreal KisPixelBrushOpacityOption::getDynamicOpacity(const KisPaintInformation& info) const
{
    return computeValue(info);
}

void KisPixelBrushOpacityOption::setOpacity(qreal opacity)
{
    setValue(opacity);
}

void KisPixelBrushOpacityOption::apply(KisPainter* painter, const KisPaintInformation& info)
{
    if (m_paintActionType == WASH && m_nodeHasIndirectPaintingSupport)
        painter->setOpacityUpdateAverage(quint8(getDynamicOpacity(info) * 255.0));
    else
        painter->setOpacityUpdateAverage(quint8(getStaticOpacity() * getDynamicOpacity(info) * 255.0));

}
