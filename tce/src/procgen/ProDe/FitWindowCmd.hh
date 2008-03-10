/**
 * @file FitWindowCmd.hh
 *
 * Declaration of FitWindowCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_FIT_WINDOW_CMD_HH
#define TTA_FIT_WINDOW_CMD_HH

#include "EditorCommand.hh"

/**
 * EditorCommand for zooming the canvas to fit the window.
 */
class FitWindowCmd : public EditorCommand {
public:
    FitWindowCmd();
    virtual ~FitWindowCmd();
    virtual bool Do();
    virtual int id() const;
    virtual FitWindowCmd* create() const;
    virtual std::string icon() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};
#endif


