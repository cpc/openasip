/**
 * @file MachCommand.hh
 *
 * Declaration of MachCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MACH_COMMAND
#define TTA_MACH_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"

/**
 * Implementation of the "mach" command of the Simulator Control Language.
 */
class MachCommand : public CustomCommand {
public:
    MachCommand();
    virtual ~MachCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
