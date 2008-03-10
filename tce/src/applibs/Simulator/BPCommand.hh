/**
 * @file BPCommand.hh
 *
 * Declaration of BPCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BP_COMMAND
#define TTA_BP_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"

/**
 * Implementation of the "bp" command of the Simulator Control Language.
 */
class BPCommand : public SimControlLanguageCommand {
public:
    BPCommand();
    virtual ~BPCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
