/**
 * @file ConfCommand.hh
 *
 * Declaration of ConfCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_CONF_COMMAND
#define TTA_CONF_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"

/**
 * Implementation of the "conf" command of the Simulator Control Language.
 */
class ConfCommand : public CustomCommand {
public:
    ConfCommand();
    virtual ~ConfCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
