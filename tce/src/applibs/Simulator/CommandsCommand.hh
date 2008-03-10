/**
 * @file CommandsCommand.hh
 *
 * Declaration of CommandsCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_COMMANDS_COMMAND
#define TTA_COMMANDS_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"

/**
 * Implementation of the "commands" command of the Simulator Control Language.
 */
class CommandsCommand : public SimControlLanguageCommand {
public:
    CommandsCommand();
    virtual ~CommandsCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
