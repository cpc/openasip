/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file PrintCmd.cc
 *
 * Definition of PrintCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#include <string>
#include <wx/docview.h>

#include "ErrorDialog.hh"
#include "PrintCmd.hh"
#include "ProDe.hh"
#include "ProDeConstants.hh"
#include "tce_config.h"

using std::string;

/**
 * The Constructor.
 */
PrintCmd::PrintCmd() : EditorCommand(ProDeConstants::CMD_NAME_PRINT) {}

/**
 * The Destructor.
 */
PrintCmd::~PrintCmd() {}

/**
 * Executes the command.
 *
 * If the wxWidgets printing framework was not compiled in the wxWidgets
 * library, printing is not possible. An error message will be displayed
 * instead of printing.
 *
 * @return Always false. The command is not undoable.
 */
bool PrintCmd::Do() {

#if WX_PRINTING_DISABLED

  // wxWidgets was compiled without printing framework, display an
  // error message.

  wxString message =
      wxString(_T("Printing framework is disabled in the wxWidgets\n"));

  message.Append(_T("library. Print previewing is not possible."));

  ErrorDialog error(parentWindow(), message);
  error.ShowModal();
  return false;

#else

  // wxWidgets was compiled with the printing framework, create printout
  // of the active view and print it.

  // find the active view
  wxView *view = wxGetApp().docManager()->GetCurrentView();
  if (view == NULL) {
    return false;
  }

  // create a printout of the active view
  wxPrintout *printout = view->OnCreatePrintout();

  if (printout != NULL) {
    wxPrinter printer;
    printer.Print(view->GetFrame(), printout, true);

    delete printout;
  }

  return false;

#endif // WX_PRINTING_DISABLED
}

/**
 * Return id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int PrintCmd::id() const { return ProDeConstants::COMMAND_PRINT; }

/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
PrintCmd *PrintCmd::create() const { return new PrintCmd(); }

/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string PrintCmd::icon() const { return ProDeConstants::CMD_ICON_PRINT; }

/**
 * Returns true when command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if the command is executable.
 */
bool PrintCmd::isEnabled() {
  wxDocManager *manager = wxGetApp().docManager();
  if (manager->GetCurrentView() != NULL) {
    return true;
  }
  return false;
}
