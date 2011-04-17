/* This file is part of the KDE project
 * Copyright (C) 2007,2011 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2008 Rob Buis <buis@kde.org>
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

#include "AddTextRangeCommand.h"
#include "ArtisticTextShape.h"
#include <KLocale>

AddTextRangeCommand::AddTextRangeCommand(ArtisticTextTool *tool, ArtisticTextShape *shape, const QString &text, int from)
: m_tool(tool), m_shape(shape), m_text(text), m_from(from)
{
    setText( i18n("Add text range") );
}

void AddTextRangeCommand::redo()
{
    QUndoCommand::redo();

    if ( !m_shape )
        return;

    m_shape->insertText(m_from, m_text);

    if (m_tool) {
        m_tool->setTextCursor(m_shape, m_from + m_text.length());
    }
}

void AddTextRangeCommand::undo()
{
    QUndoCommand::undo();

    if ( ! m_shape )
        return;

    if (m_tool) {
        m_tool->setTextCursor(m_shape, m_from);
    }

    m_shape->removeText(m_from, m_text.length());
}
