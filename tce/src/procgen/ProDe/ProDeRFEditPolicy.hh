/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file ProDeRFEditPolicy.hh
 *
 * Declaration of ProDeRFEditPolicy class.
 *
 * @author Ari Mets�halme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 20 2004 by vpj, jn, am
 */

#ifndef TTA_PRODE_RF_EDIT_POLICY_HH
#define TTA_PRODE_RF_EDIT_POLICY_HH


#include "EditPolicy.hh"

class Request;
class ComponentCommand;

/**
 * Determines how a register file EditPart acts when a Request is performed
 * on it.
 *
 * Converts a given Request to a Command if the EditPolicy supports
 * the Request.
 */
class ProDeRFEditPolicy : public EditPolicy {
public:
    ProDeRFEditPolicy();
    virtual ~ProDeRFEditPolicy();

    virtual ComponentCommand* getCommand(Request* request);
    virtual bool canHandle(Request* request) const;

private:
    /// Assignment not allowed.
    ProDeRFEditPolicy& operator=(ProDeRFEditPolicy& old);
    /// Copying not allowed.
    ProDeRFEditPolicy(ProDeRFEditPolicy& old);
};

#endif
