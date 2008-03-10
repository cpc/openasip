/**
 * @file NextiCommand.hh
 *
 * Declaration of NextiCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_NEXTI_COMMAND
#define TTA_NEXTI_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"

/**
 * Implementation of the "nexti" command of the Simulator Control Language.
 */
class NextiCommand : public SimControlLanguageCommand {
public:
    NextiCommand();
    virtual ~NextiCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
