/**
 * @file FitHeightCmd.hh
 *
 * Declaration of FitHeightCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_FIT_HEIGHT_CMD_HH
#define TTA_FIT_HEIGHT_CMD_HH

#include "EditorCommand.hh"

/**
 * EditorCommand for zooming the canvas to fit the window height.
 */
class FitHeightCmd : public EditorCommand {
public:
    FitHeightCmd();
    virtual ~FitHeightCmd();
    virtual bool Do();
    virtual int id() const;
    virtual FitHeightCmd* create() const;
    virtual std::string icon() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};
#endif
