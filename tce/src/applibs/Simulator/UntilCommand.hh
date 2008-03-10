/**
 * @file UntilCommand.hh
 *
 * Declaration of UntilCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_UNTIL_COMMAND
#define TTA_UNTIL_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"

/**
 * Implementation of the "until" command of the Simulator Control Language.
 */
class UntilCommand : public SimControlLanguageCommand {
public:
    UntilCommand();
    virtual ~UntilCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
