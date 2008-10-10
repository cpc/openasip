/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file RootEditPart.hh
 *
 * Declaration of RootEditPart class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
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
