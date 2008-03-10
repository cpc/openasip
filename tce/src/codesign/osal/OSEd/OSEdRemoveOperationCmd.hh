/**
 * @file OSEdRemoveOperationCmd.hh
 *
 * Declaration of OSEdRemoveModuleCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_REMOVE_OPERATION_CMD_HH
#define TTA_OSED_REMOVE_OPERATION_CMD_HH

#include <string>

#include "GUICommand.hh"

/**
 * Command for removing an operation.
 */
class OSEdRemoveOperationCmd : public GUICommand {
public:
    OSEdRemoveOperationCmd();
    virtual ~OSEdRemoveOperationCmd();

    virtual int id() const;
    virtual GUICommand* create() const;
    virtual bool Do();
    virtual bool isEnabled();
    virtual std::string icon() const;

private:
    /// Copying not allowed.
    OSEdRemoveOperationCmd(const OSEdRemoveOperationCmd&);
    /// Assignment not allowed.
    OSEdRemoveOperationCmd& operator=(const OSEdRemoveOperationCmd&);
};

#endif
