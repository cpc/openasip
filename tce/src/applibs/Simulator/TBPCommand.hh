/**
 * @file TBPCommand.hh
 *
 * Declaration of TBPCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_TBP_COMMAND
#define TTA_TBP_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"

/**
 * Implementation of the "tbp" command of the Simulator Control Language.
 */
class TBPCommand : public SimControlLanguageCommand {
public:
    TBPCommand();
    virtual ~TBPCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
