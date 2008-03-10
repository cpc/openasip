/**
 * @file KillCommand.hh
 *
 * Declaration of KillCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_KILL_COMMAND
#define TTA_KILL_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "SimControlLanguageCommand.hh"
#include "Exception.hh"

/**
 * Implementation of the "kill" command of the Simulator Control Language.
 */
class KillCommand : public SimControlLanguageCommand {
public:
    KillCommand();
    virtual ~KillCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
