/**
 * @file ProgCommand.hh
 *
 * Declaration of ProgCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROG_COMMAND
#define TTA_PROG_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"

/**
 * Implementation of the "prog" command of the Simulator Control Language.
 */
class ProgCommand : public SimControlLanguageCommand {
public:
    ProgCommand();
    virtual ~ProgCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
    
};
#endif
