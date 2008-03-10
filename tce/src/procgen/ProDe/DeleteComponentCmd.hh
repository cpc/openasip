/**
 * @file DeleteComponentCmd.hh
 *
 * Declaration of DeleteComponentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_DELETE_COMPONENT_CMD_HH
#define TTA_DELETE_COMPONENT_CMD_HH

#include <wx/wx.h>
#include <wx/cmdproc.h>
#include "EditorCommand.hh"


/**
 * wxCommand for deleting components from the Machine.
 */
class DeleteComponentCmd: public EditorCommand {
public:
    DeleteComponentCmd();
    virtual bool Do();
    virtual int id() const;
    virtual DeleteComponentCmd* create() const;
    virtual std::string shortName() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
};

#endif
