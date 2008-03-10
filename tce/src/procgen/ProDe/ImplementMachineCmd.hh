/**
 * @file ImplementMachineCmd.hh
 *
 * Declaration of ImplementMachineCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_IMPLEMENT_CMD_HH
#define TTA_IMPLEMENT_CMD_HH

#include <string>

#include "EditorCommand.hh"

/**
 * Command for opening processor implementation window.
 */
class ImplementMachineCmd : public EditorCommand {
public:
    ImplementMachineCmd();
    virtual ~ImplementMachineCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ImplementMachineCmd* create() const;
    virtual std::string icon() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};

#endif
