/* ASEPRITE
 * Copyright (C) 2001-2012  David Capello
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"

#include "app.h"
#include "app/find_widget.h"
#include "app/load_widget.h"
#include "commands/command.h"
#include "context.h"
#include "modules/editors.h"
#include "settings/document_settings.h"
#include "settings/settings.h"
#include "ui/window.h"
#include "ui_context.h"
#include "widgets/status_bar.h"

#include <allegro/unicode.h>

using namespace ui;
using namespace gfx;

//////////////////////////////////////////////////////////////////////
// show_grid

class ShowGridCommand : public Command
{
public:
  ShowGridCommand()
    : Command("ShowGrid",
              "Show Grid",
              CmdUIOnlyFlag)
  {
  }

  Command* clone() const { return new ShowGridCommand(*this); }

protected:
  bool onChecked(Context* context)
  {
    IDocumentSettings* docSettings = context->getSettings()->getDocumentSettings(context->getActiveDocument());

    return docSettings->getGridVisible();
  }

  void onExecute(Context* context)
  {
    IDocumentSettings* docSettings = context->getSettings()->getDocumentSettings(context->getActiveDocument());

    docSettings->setGridVisible(docSettings->getGridVisible() ? false: true);

    refresh_all_editors();      // TODO this should be done by "setGridVisible" impl
  }
};

//////////////////////////////////////////////////////////////////////
// snap_to_grid

class SnapToGridCommand : public Command
{
public:
  SnapToGridCommand()
    : Command("SnapToGrid",
              "Snap to Grid",
              CmdUIOnlyFlag)
  {
  }

  Command* clone() const { return new SnapToGridCommand(*this); }

protected:
  bool onChecked(Context* context)
  {
    IDocumentSettings* docSettings = context->getSettings()->getDocumentSettings(context->getActiveDocument());

    return docSettings->getSnapToGrid();
  }

  void onExecute(Context* context)
  {
    IDocumentSettings* docSettings = context->getSettings()->getDocumentSettings(context->getActiveDocument());
    char buf[512];

    docSettings->setSnapToGrid(docSettings->getSnapToGrid() ? false: true);

    usprintf(buf, "Snap to grid: %s",
             (docSettings->getSnapToGrid() ? "On": "Off"));

    StatusBar::instance()->setStatusText(250, buf);
  }
};

//////////////////////////////////////////////////////////////////////
// grid_settings

class GridSettingsCommand : public Command
{
public:
  GridSettingsCommand();
  Command* clone() const { return new GridSettingsCommand(*this); }

protected:
  bool onEnabled(Context* context);
  void onExecute(Context* context);
};

GridSettingsCommand::GridSettingsCommand()
  : Command("GridSettings",
            "Grid Settings",
            CmdUIOnlyFlag)
{
}

bool GridSettingsCommand::onEnabled(Context* context)
{
  return true;
}

void GridSettingsCommand::onExecute(Context* context)
{
  UniquePtr<Window> window(app::load_widget<Window>("grid_settings.xml", "grid_settings"));
  Widget* button_ok = app::find_widget<Widget>(window, "ok");
  Widget* grid_x = app::find_widget<Widget>(window, "grid_x");
  Widget* grid_y = app::find_widget<Widget>(window, "grid_y");
  Widget* grid_w = app::find_widget<Widget>(window, "grid_w");
  Widget* grid_h = app::find_widget<Widget>(window, "grid_h");

  IDocumentSettings* docSettings = context->getSettings()->getDocumentSettings(context->getActiveDocument());
  Rect bounds = docSettings->getGridBounds();

  grid_x->setTextf("%d", bounds.x);
  grid_y->setTextf("%d", bounds.y);
  grid_w->setTextf("%d", bounds.w);
  grid_h->setTextf("%d", bounds.h);

  window->openWindowInForeground();

  if (window->get_killer() == button_ok) {
    bounds.x = grid_x->getTextInt();
    bounds.y = grid_y->getTextInt();
    bounds.w = grid_w->getTextInt();
    bounds.h = grid_h->getTextInt();
    bounds.w = MAX(bounds.w, 1);
    bounds.h = MAX(bounds.h, 1);

    docSettings->setGridBounds(bounds);

    if (docSettings->getGridVisible())
        refresh_all_editors();      // TODO this should be done by "setGridBounds" impl
  }
}

//////////////////////////////////////////////////////////////////////
// CommandFactory

Command* CommandFactory::createShowGridCommand()
{
  return new ShowGridCommand;
}

Command* CommandFactory::createSnapToGridCommand()
{
  return new SnapToGridCommand;
}

Command* CommandFactory::createGridSettingsCommand()
{
  return new GridSettingsCommand;
}
