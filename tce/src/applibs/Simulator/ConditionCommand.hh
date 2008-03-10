/**
 * @file ConditionCommand.hh
 *
 * Declaration of ConditionCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_CONDITION_COMMAND
#define TTA_CONDITION_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"

/**
 * Implementation of the "condition" command of the Simulator Control Language.
 */
class ConditionCommand : public SimControlLanguageCommand {
public:
    ConditionCommand();
    virtual ~ConditionCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
