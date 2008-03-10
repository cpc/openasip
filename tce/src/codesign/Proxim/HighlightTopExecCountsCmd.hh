/**
 * @file HighlightTopExecCountsCmd.hh
 *
 * Declaration of HighlightTopExecCountsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_HIGHLIGHT_TOP_EXEC_COUNTS_CMD_HH
#define TTA_HIGHLIGHT_TOP_EXEC_COUNTS_CMD_HH

#include "GUICommand.hh"

/**
 * Command for highlighting instructions with top execution counts in the
 * disassembly window.
 */
class HighlightTopExecCountsCmd : public GUICommand {
public:
    HighlightTopExecCountsCmd();
    virtual ~HighlightTopExecCountsCmd();
    virtual bool Do();
    virtual int id() const;
    virtual HighlightTopExecCountsCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};
#endif
