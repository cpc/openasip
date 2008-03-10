/**
 * @file RunCommand.hh
 *
 * Declaration of RunCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_RUN_COMMAND
#define TTA_RUN_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"

/**
 * Implementation of the "run" command of the Simulator Control Language.
 */
class RunCommand : public SimControlLanguageCommand {
public:
    RunCommand();
    virtual ~RunCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
