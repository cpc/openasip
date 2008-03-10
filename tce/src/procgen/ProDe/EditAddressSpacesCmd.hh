/**
 * @file EditAddressSpacesCmd.hh
 *
 * Declaration of EditAddressSpacesCmd class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen@tut.fi)
 */

#ifndef TTA_EDIT_ADDRESS_SPACES_CMD_HH
#define TTA_EDIT_ADDRESS_SPACES_CMD_HH

#include "EditorCommand.hh"

/**
 * wxCommand for editing address spaces.
 */
class EditAddressSpacesCmd : public EditorCommand {
public:
    EditAddressSpacesCmd();
    virtual ~EditAddressSpacesCmd();
    virtual bool Do();
    virtual int id() const;
    virtual EditAddressSpacesCmd* create() const;
    virtual std::string icon() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};

#endif
