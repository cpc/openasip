/**
 * @file FitWidthCmd.hh
 *
 * Declaration of FitWidthCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_FIT_WIDTH_CMD_HH
#define TTA_FIT_WIDTH_CMD_HH

#include "EditorCommand.hh"

/**
 * EditorCommand for zooming the canvas to fit the window width.
 */
class FitWidthCmd : public EditorCommand {
public:
    FitWidthCmd();
    virtual ~FitWidthCmd();
    virtual bool Do();
    virtual int id() const;
    virtual FitWidthCmd* create() const;
    virtual std::string icon() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};
#endif


