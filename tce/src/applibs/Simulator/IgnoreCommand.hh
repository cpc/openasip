/**
 * @file IgnoreCommand.hh
 *
 * Declaration of IgnoreCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_IGNORE_COMMAND
#define TTA_IGNORE_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"

/**
 * Implementation of the "ignore" command of the Simulator Control Language.
 */
class IgnoreCommand : public SimControlLanguageCommand {
public:
    IgnoreCommand();
    virtual ~IgnoreCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
