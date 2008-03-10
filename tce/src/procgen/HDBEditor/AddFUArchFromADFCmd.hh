/**
 * @file AddFUArchFromADFCmd.hh
 *
 * Declaration of AddFUArchFromADFCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_FU_ARCH_FROM_ADF_CMD_HH
#define TTA_ADD_FU_ARCH_FROM_ADF_CMD_HH

#include "GUICommand.hh"

/**
 * Command for adding new FU architectures to HDB from an ADF file.
 */
class AddFUArchFromADFCmd : public GUICommand {
public:
    AddFUArchFromADFCmd();
    virtual ~AddFUArchFromADFCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddFUArchFromADFCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};

#endif
