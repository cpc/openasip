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
 * @file EditPolicy.hh
 *
 * Declaration of EditPolicy class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 20 2004 by vpj, jn, am
 */

#ifndef TTA_EDIT_POLICY_HH
#define TTA_EDIT_POLICY_HH

class EditPart;
class Request;
class ComponentCommand;
 
/**
 * Determines how an EditPart acts when a Request is performed on it.
 *
 * Converts a given Request to a Command if the EditPolicy supports
 * the Request.
 */
class EditPolicy {
public:
    EditPolicy();
    virtual ~EditPolicy();

    EditPart* host() const;
    void setHost(EditPart* host);

    /**
     * Returns the Command corresponding to the type of the Request.
     *
     * @param request Request to be handled.
     * @return NULL if the Request cannot be handled.
     */
    virtual ComponentCommand* getCommand(Request* request) = 0;

    /**
     * Tells whether this EditPolicy is able to handle a certain type
     * of Request.
     *
     * @param request Request to be asked if it can be handled.
     * @return True if the Request can be handled, false otherwise.
     */
    virtual bool canHandle(Request* request) const = 0;

protected:
    /// Host EditPart of this EditPolicy.
    EditPart* host_;

private:
    /// Assignment not allowed.
    EditPolicy& operator=(EditPolicy& old);
    /// Copying not allowed.
    EditPolicy(EditPolicy& old);
};

#include "EditPolicy.icc"

#endif
