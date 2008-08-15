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
