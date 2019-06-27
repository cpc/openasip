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
 * @file BusFactory.hh
 *
 * Declaration of BusFactory class.
 *
 * @author Ari Mets�halme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#ifndef TTA_BUS_FACTORY_HH
#define TTA_BUS_FACTORY_HH

#include "EditPartFactory.hh"

class EditPart;

/**
 * Factory for creating bus EditParts corresponding to a given Machine
 * Bus object.
 */
class BusFactory : public EditPartFactory {
public:
    BusFactory(EditPolicyFactory& editPolicyFactory);
    virtual ~BusFactory();

    virtual EditPart* createEditPart(TTAMachine::MachinePart* component);

private:
    /// Assignment not allowed.
    BusFactory& operator=(BusFactory& old);
    /// Copying not allowed.
    BusFactory(BusFactory& old);
};

#endif
