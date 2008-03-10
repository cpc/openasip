/**
 * @file ModifySRPortCmd.hh
 *
 * Declaration of ModifySRPortCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MODIFY_SR_PORT_CMD_HH
#define TTA_MODIFY_SR_PORT_CMD_HH

#include "ComponentCommand.hh"

class EditPart;

/**
 * Command which displays a special register port dialog with precompleted
 * values for modifying a special register port.
 */
class ModifySRPortCmd: public ComponentCommand {
public:
    ModifySRPortCmd(EditPart* editPart);
    virtual ~ModifySRPortCmd();
    virtual bool Do();

private:
     /// Function unit port to modify.
     EditPart* editPart_;
};

#endif
