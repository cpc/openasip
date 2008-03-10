/**
 * @file EditBusOrderCmd.hh
 *
 * Declaration of EditBusOrderCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_EDIT_BUS_ORDER_CMD_HH
#define TTA_EDIT_BUS_ORDER_CMD_HH

#include "EditorCommand.hh"

/**
 * Command for editing order of transport buses in a machine.
 */
class EditBusOrderCmd : public EditorCommand {
public:
    EditBusOrderCmd();
    virtual ~EditBusOrderCmd();
    virtual bool Do();
    virtual int id() const;
    virtual EditBusOrderCmd* create() const;
    virtual bool isEnabled();
    virtual std::string shortName() const;
};

#endif
