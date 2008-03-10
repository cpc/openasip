/**
 * @file HighlightExecPercentageCmd.hh
 *
 * Declaration of HighlightExecPercentageCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_HIGHLIGHT_EXEC_PERCENTAGE_CMD_HH
#define TTA_HIGHLIGHT_EXEC_PERCENTAGE_CMD_HH

#include "GUICommand.hh"

/**
 * Command for highlighting move execution counts in the disassembly window.
 */
class HighlightExecPercentageCmd : public GUICommand {
public:
    HighlightExecPercentageCmd();
    virtual ~HighlightExecPercentageCmd();
    virtual bool Do();
    virtual int id() const;
    virtual HighlightExecPercentageCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};
#endif
