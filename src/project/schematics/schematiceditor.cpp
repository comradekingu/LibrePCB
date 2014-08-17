/*
 * EDA4U - Professional EDA for everyone!
 * Copyright (C) 2013 Urban Bruhin
 * http://eda4u.ubruhin.ch/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*****************************************************************************************
 *  Includes
 ****************************************************************************************/

#include <QtCore>
#include <QtWidgets>
#include "schematiceditor.h"
#include "ui_schematiceditor.h"
#include "../project.h"
#include "../../workspace/workspace.h"
#include "../../workspace/workspacesettings.h"

namespace project {

/*****************************************************************************************
 *  Constructors / Destructor
 ****************************************************************************************/

SchematicEditor::SchematicEditor(Workspace& workspace, Project& project) :
    QMainWindow(0), mWorkspace(workspace), mProject(project), mUi(new Ui::SchematicEditor)
{
    mUi->setupUi(this);

    // connect some actions which are created with the Qt Designer
    connect(mUi->actionSave_Project, SIGNAL(triggered()), &mProject, SLOT(save()));
    connect(mUi->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(mUi->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    // connect the undo/redo actions with the QUndoStack of the project
    connect(mUi->actionUndo, SIGNAL(triggered()), mProject.getUndoStack(), SLOT(undo()));
    connect(mUi->actionRedo, SIGNAL(triggered()), mProject.getUndoStack(), SLOT(redo()));
    connect(mProject.getUndoStack(), &QUndoStack::undoTextChanged,
            [this](const QString& text)
            {mUi->actionUndo->setText(text.isEmpty() ? tr("Undo") : text);});
    //mUi->actionUndo->setText(mProject.getUndoStack()->undoText()); // would clear the text!
    connect(mProject.getUndoStack(), &QUndoStack::canUndoChanged,
            [this](bool can){mUi->actionUndo->setEnabled(can);});
    mUi->actionUndo->setEnabled(mProject.getUndoStack()->canUndo());
    connect(mProject.getUndoStack(), &QUndoStack::redoTextChanged,
            [this](const QString& text)
            {mUi->actionRedo->setText(text.isEmpty() ? tr("Redo") : text);});
    //mUi->actionRedo->setText(mProject.getUndoStack()->redoText()); // would clear the text!
    connect(mProject.getUndoStack(), &QUndoStack::canRedoChanged,
            [this](bool can){mUi->actionRedo->setEnabled(can);});
    mUi->actionRedo->setEnabled(mProject.getUndoStack()->canRedo());

    // Restore Window Geometry
    QSettings s(mWorkspace.getMetadataPath().getPathTo("settings.ini").toStr(), QSettings::IniFormat);
    restoreGeometry(s.value("schematic_editor/window_geometry").toByteArray());
    restoreState(s.value("schematic_editor/window_state").toByteArray());
}

SchematicEditor::~SchematicEditor()
{
    // Save Window Geometry
    QSettings s(mWorkspace.getMetadataPath().getPathTo("settings.ini").toStr(), QSettings::IniFormat);
    s.setValue("schematic_editor/window_geometry", saveGeometry());
    s.setValue("schematic_editor/window_state", saveState());

    delete mUi;     mUi = 0;
}

/*****************************************************************************************
 *  Inherited Methods
 ****************************************************************************************/

void SchematicEditor::closeEvent(QCloseEvent* event)
{
    if (!mProject.windowIsAboutToClose(this))
        event->ignore();
    else
        QMainWindow::closeEvent(event);
}

/*****************************************************************************************
 *  Actions
 ****************************************************************************************/

void SchematicEditor::on_actionClose_Project_triggered()
{
    mProject.close(this);
}

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace project