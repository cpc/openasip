/**
 * @file CmdHelp.hh
 *
 * Declaration of CmdHelp class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note reviewed 2 June 2004 by jm, pj, tr, jn
 * @note rating: green
 */

#ifndef TTA_CMD_HELP_HH
#define TTA_CMD_HELP_HH

#include <vector>
#include <string>

#include "CustomCommand.hh"
#include "DataObject.hh"


/**
 * Help command.
 *
 * Prints the help text of the CustomCommand that is given as parameter
 * to execute.
 */
class CmdHelp : public CustomCommand {

public:
    CmdHelp();
    CmdHelp(const CmdHelp& cmd);
    virtual ~CmdHelp();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;

private:
    /// Assignment not allowed.
    CmdHelp& operator=(const CmdHelp&);
};

#endif
