/**
 * @file HelpCommand.hh
 *
 * Declaration of HelpCommand class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_HELP_COMMAND_HH
#define TTA_HELP_COMMAND_HH

#include <vector>
#include <string>

#include "DataObject.hh"
#include "SimControlLanguageCommand.hh"

/**
 * Implementation of the "help" command of the Simulator Control Language.
 */
class HelpCommand : public SimControlLanguageCommand {
public:
    HelpCommand();
    virtual ~HelpCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);

    virtual std::string helpText() const;
private:
    /// Assignment not allowed.
    HelpCommand& operator=(const HelpCommand&);
    /// Copying not allowed.
    HelpCommand(const HelpCommand& cmd);
};

#endif
