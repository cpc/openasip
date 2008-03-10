/**
 * @file ProximOpenProgramCmd.hh
 *
 * Declaration of ProximOpenProgramCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_OPEN_PROGRAM_CMD_HH
#define TTA_PROXIM_OPEN_PROGRAM_CMD_HH

#include "GUICommand.hh"

/**
 * Command for opening program files in the Proxim.
 *
 * Displays a file dialog for choosing the program file to open.
 */
class ProximOpenProgramCmd : public GUICommand {
public:
    ProximOpenProgramCmd();
    virtual ~ProximOpenProgramCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ProximOpenProgramCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
    virtual std::string shortName() const;
};
#endif
