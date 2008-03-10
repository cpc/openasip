/**
 * @file RootEditPart.hh
 *
 * Declaration of RootEditPart class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 13 2004 by vpj, ll, jn, am
 */

#ifndef TTA_ROOT_EDIT_PART_HH
#define TTA_ROOT_EDIT_PART_HH

#include "EditPart.hh"

/**
 * The root EditPart of the whole processor view.
 *
 * Contains contents EditPart.
 */
class RootEditPart : public EditPart {
public:
    RootEditPart();
    virtual ~RootEditPart();

    EditPart* contents() const;
    void setContents(EditPart* contents);

private:
    /// Assignment not allowed.
    RootEditPart& operator=(RootEditPart& old);
    /// Copying not allowed.
    RootEditPart(RootEditPart& old);

    void collectTrash();
    /// Contents EditPart which parents all EditParts in the View.
    EditPart* contents_;
};

#include "RootEditPart.icc"

#endif
