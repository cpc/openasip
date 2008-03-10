/**
 * @file OSEdAddModuleCmd.hh
 *
 * Declaration of OSEdAddModuleCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_ADD_MODULE_CMD_HH
#define TTA_OSED_ADD_MODULE_CMD_HH

#include <string>

#include "GUICommand.hh"

/**
 * Command for adding new module to operation set.
 */
class OSEdAddModuleCmd : public GUICommand {
public:
    OSEdAddModuleCmd();
    virtual ~OSEdAddModuleCmd();

    virtual int id() const;
    virtual GUICommand* create() const;
    virtual bool Do();
    virtual bool isEnabled();
    virtual std::string icon() const;

private:
    /// Copying not allowed.
    OSEdAddModuleCmd(const OSEdAddModuleCmd&);
    /// Assignment not allowed.
    OSEdAddModuleCmd& operator=(const OSEdAddModuleCmd&);
};

#endif
