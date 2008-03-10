/**
 * @file ProximCDCmd.hh
 *
 * Declaration of ProximCDCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_CD_CMD_HH
#define TTA_PROXIM_CD_CMD_HH

#include "GUICommand.hh"

/**
 * Command for changing the Proxim working directory.
 *
 * The command displays a directory dialog for choosing a new
 * working directory.
 */
class ProximCDCmd : public GUICommand {
public:
    ProximCDCmd();
    virtual ~ProximCDCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ProximCDCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};
#endif
