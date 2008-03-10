/**
 * @file DeleteBPCommand.hh
 *
 * Declaration of DeleteBPCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DELETEBP_COMMAND
#define TTA_DELETEBP_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"

/**
 * Implementation of the "deletebp" command of the Simulator Control Language.
 */
class DeleteBPCommand : public SimControlLanguageCommand {
public:
    DeleteBPCommand();
    virtual ~DeleteBPCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
