/**
 * @file InfoCommand.hh
 *
 * Declaration of InfoCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_INFO_COMMAND
#define TTA_INFO_COMMAND

#include <string>
#include <vector>
#include <map>

#include "DataObject.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"


/**
 * Implementation of the "info" command of the Simulator Control Language.
 */
class InfoCommand : public SimControlLanguageCommand {
public:
    InfoCommand(bool isCompiledSimulation = false);
    virtual ~InfoCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
protected:
    /// container type for subcommands indexed by name
    typedef std::map<std::string, SimControlLanguageSubCommand*> SubCommandMap;
    /// storage for subcommands
    SubCommandMap subCommands_;
    
    /// Is it a compiled simulation?
    bool isCompiledSimulation_;
};
#endif
