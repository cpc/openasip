/**
 * @file SetStatusTextCmd.cc
 *
 * Definition of SetStatusTextCmdCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <wx/docview.h>
#include "Application.hh"
#include "SetStatusTextCmd.hh"
#include "EditPart.hh"
#include "ProDe.hh"
#include "WxConversion.hh"
#include "ChildFrame.hh"

using std::string;

/**
 * The Constructor.
 *
 * @param text Status text to set.
 */
SetStatusTextCmd::SetStatusTextCmd(string text):
    ComponentCommand(), text_(text) {
}


/**
 * The Destructor.
 */
SetStatusTextCmd::~SetStatusTextCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
SetStatusTextCmd::Do() {
    ChildFrame* frame = dynamic_cast<ChildFrame*>(
        wxGetApp().docManager()->GetCurrentView()->GetFrame());
    frame->setStatus(WxConversion::toWxString(text_));
    return true;
}
