/**
 * @file ConnectionEditPart.hh
 *
 * Declaration of ConnectionEditPart class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 13 2004 by vpj, ll, jn, am
 */

#ifndef TTA_CONNECTION_EDIT_PART_HH
#define TTA_CONNECTION_EDIT_PART_HH

#include "EditPart.hh"

/**
 * A directed connection between two machine blocks on the canvas.
 */
class ConnectionEditPart : public EditPart {
public:
    ConnectionEditPart();
    virtual ~ConnectionEditPart();

    EditPart* source() const;
    EditPart* target() const;

    void setSource(EditPart* source);
    void setTarget(EditPart* target);

private:
    /// Assignment not allowed.
    ConnectionEditPart& operator=(ConnectionEditPart& old);
    /// Copying not allowed.
    ConnectionEditPart(ConnectionEditPart& old);

    /// Source of connection.
    EditPart* source_;
    /// Target of connection.
    EditPart* target_;
};

#include "ConnectionEditPart.icc"

#endif
