/**
 * @file ResumeCommand.hh
 *
 * Declaration of ResumeCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_RESUME_COMMAND
#define TTA_RESUME_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"

/**
 * Implementation of the "resume" command of the Simulator Control Language.
 */
class ResumeCommand : public SimControlLanguageCommand {
public:
    ResumeCommand();
    virtual ~ResumeCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
