/**
 * @file EnableBPCommand.hh
 *
 * Declaration of EnableBPCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ENABLEBP_COMMAND
#define TTA_ENABLEBP_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"

/**
 * Implementation of the "enablebp" command of the Simulator Control Language.
 */
class EnableBPCommand : public SimControlLanguageCommand {
public:
    EnableBPCommand();
    virtual ~EnableBPCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
