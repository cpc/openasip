/**
 * @file ZoomOutCmd.hh
 *
 * Declaration of ZoomOutCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_ZOOM_OUT_CMD_HH
#define TTA_ZOOM_OUT_CMD_HH

#include "EditorCommand.hh"

/**
 * EditorCommand for zooming out the canvas.
 */
class ZoomOutCmd : public EditorCommand {
public:
    ZoomOutCmd();
    virtual ~ZoomOutCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ZoomOutCmd* create() const;
    virtual std::string icon() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};
#endif

