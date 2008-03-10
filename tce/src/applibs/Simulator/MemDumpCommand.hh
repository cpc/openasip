/**
 * @file MemDumpCommand.hh
 *
 * Declaration of MemDumpCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MEMDUMP_COMMAND
#define TTA_MEMDUMP_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"

/**
 * Implementation of the "x" command of the Simulator Control Language.
 */
class MemDumpCommand : public SimControlLanguageCommand {
public:
    MemDumpCommand();
    virtual ~MemDumpCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
