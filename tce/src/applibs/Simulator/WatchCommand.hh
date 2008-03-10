/**
 * @file WatchCommand.hh
 *
 * Declaration of WatchCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_WATCH_COMMAND
#define TTA_WATCH_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"

/**
 * Implementation of the "watch" command of the Simulator Control Language.
 */
class WatchCommand : public SimControlLanguageCommand {
public:
    WatchCommand();
    virtual ~WatchCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
