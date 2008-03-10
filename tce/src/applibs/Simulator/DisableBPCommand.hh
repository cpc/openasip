/**
 * @file DisableBPCommand.hh
 *
 * Declaration of DisableBPCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISABLEBP_COMMAND
#define TTA_DISABLEBP_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"

/**
 * Implementation of the "disablebp" command of the Simulator Control Language.
 */
class DisableBPCommand : public SimControlLanguageCommand {
public:
    DisableBPCommand();
    virtual ~DisableBPCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
