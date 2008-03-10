/**
 * @file ClearDisassemblyHighlightCmd.hh
 *
 * Declaration of ClearDisassemblyHighlightCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_CLEAR_DISASSEMBLY_HIGHLIGHT_CMD_HH
#define TTA_CLEAR_DISASSEMBLY_HIGHLIGHT_CMD_HH

#include "GUICommand.hh"

/**
 * Command for clearing disassembly window highlighting mode.
 */
class ClearDisassemblyHighlightCmd : public GUICommand {
public:
    ClearDisassemblyHighlightCmd();
    virtual ~ClearDisassemblyHighlightCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ClearDisassemblyHighlightCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};
#endif
