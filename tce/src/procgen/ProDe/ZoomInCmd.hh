/**
 * @file ZoomInCmd.hh
 *
 * Declaration of ZoomInCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_ZOOM_IN_CMD_HH
#define TTA_ZOOM_IN_CMD_HH

#include "EditorCommand.hh"

/**
 * EditorCommand for zooming in the canvas.
 */
class ZoomInCmd : public EditorCommand {
public:
    ZoomInCmd();
    virtual ~ZoomInCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ZoomInCmd* create() const;
    virtual std::string icon() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};
#endif


