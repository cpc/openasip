/**
 * @file StepiCommand.hh
 *
 * Declaration of StepiCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_STEPI_COMMAND
#define TTA_STEPI_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"

/**
 * Implementation of the "stepi" command of the Simulator Control Language.
 */
class StepiCommand : public SimControlLanguageCommand {
public:
    StepiCommand();
    virtual ~StepiCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
