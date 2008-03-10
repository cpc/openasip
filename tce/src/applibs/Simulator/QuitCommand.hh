/**
 * @file QuitCommand.hh
 *
 * Declaration of QuitCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_QUIT_COMMAND
#define TTA_QUIT_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"

/**
 * Implementation of the "quit" command of the Simulator Control Language.
 */
class QuitCommand : public CustomCommand {
public:
    QuitCommand();
    virtual ~QuitCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
