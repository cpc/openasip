/**
 * @file OSEdRemoveModuleCmd.hh
 *
 * Declaration of OSEdRemoveModuleCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_REMOVE_MODULE_CMD_HH
#define TTA_OSED_REMOVE_MODULE_CMD_HH

#include <string>

#include "GUICommand.hh"

/**
 * Command for removing a module.
 */
class OSEdRemoveModuleCmd : public GUICommand {
public:
    OSEdRemoveModuleCmd();
    virtual ~OSEdRemoveModuleCmd();

    virtual int id() const;
    virtual GUICommand* create() const;
    virtual bool Do();
    virtual bool isEnabled();
    virtual std::string icon() const;

private:
    /// Copying not allowed.
    OSEdRemoveModuleCmd(const OSEdRemoveModuleCmd&);
    /// Assignment not allowed.
    OSEdRemoveModuleCmd& operator=(const OSEdRemoveModuleCmd&);
};

#endif
