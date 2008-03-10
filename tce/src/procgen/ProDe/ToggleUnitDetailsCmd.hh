/**
 * @file ToggleUnitDetailsCmd.hh
 *
 * Declaration of ToggleUnitDetailsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_TOGGLE_UNIT_DETAILS_CMD_HH
#define TTA_TOGGLE_UNIT_DETAILS_CMD_HH

#include "EditorCommand.hh"

/**
 * EditorCommand for toggling unit info string visibility in the canvas.
 */
class ToggleUnitDetailsCmd : public EditorCommand {
public:
    ToggleUnitDetailsCmd();
    virtual ~ToggleUnitDetailsCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ToggleUnitDetailsCmd* create() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
    virtual bool isChecked() const;
};
#endif
