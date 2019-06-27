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
 * @file ProDeSRPortEditPolicy.hh
 *
 * Declaration of ProDeSRPortEditPolicy class.
 *
 * @author Veli-Pekka J��skel�inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PRODE_SR_PORT_EDIT_POLICY_HH
#define TTA_PRODE_SR_PORT_EDIT_POLICY_HH

#include "EditPolicy.hh"

class Request;
class ComponentCommand;

/**
 * Determines how a special register port EditPart acts when a Request is
 * performed on it.
 *
 * Converts a given Request to a Command if the EditPolicy supports
 * the Request.
 */
class ProDeSRPortEditPolicy : public EditPolicy {
public:
    ProDeSRPortEditPolicy();
    virtual ~ProDeSRPortEditPolicy();

    virtual ComponentCommand* getCommand(Request* request);
    virtual bool canHandle(Request* request) const;

private:
    /// Assignment not allowed.
    ProDeSRPortEditPolicy& operator=(ProDeSRPortEditPolicy& old);
    /// Copying not allowed.
    ProDeSRPortEditPolicy(ProDeSRPortEditPolicy& old);
};

#endif
