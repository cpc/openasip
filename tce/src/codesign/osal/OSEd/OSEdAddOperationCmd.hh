/**
 * @file OSEdAddOperationCmd.hh
 *
 * Declaration of OSEdAddOperationCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_OPERATION_CMD_HH
#define TTA_ADD_OPERATION_CMD_HH

#include <string>

#include "GUICommand.hh"

/**
 * Command for adding new operation to operation set.
 */
class OSEdAddOperationCmd : public GUICommand {
public:
    OSEdAddOperationCmd();
    virtual ~OSEdAddOperationCmd();

    virtual int id() const;
    virtual GUICommand* create() const;
    virtual bool Do();
    virtual bool isEnabled();
    virtual std::string icon() const;

private:
    /// Copying not allowed.
    OSEdAddOperationCmd(const OSEdAddOperationCmd&);
    /// Assignment not allowed.
    OSEdAddOperationCmd operator=(OSEdAddOperationCmd&);
};

#endif
