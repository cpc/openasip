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
 * @file EditPartFactory.hh
 *
 * Declaration of EditPartFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 13 2004 by vpj, ll, jn, am
 */

#ifndef TTA_EDIT_PART_FACTORY_HH
#define TTA_EDIT_PART_FACTORY_HH

#include <vector>

#include "Factory.hh"

class EditPart;
class EditPolicyFactory;

/**
 * Abstract base class for EditPart factories that create appropriate
 * EditParts from Machine components.
 */
class EditPartFactory : public Factory {
public:
    virtual ~EditPartFactory();
    void registerFactory(Factory* factory);

    EditPartFactory& operator=(EditPartFactory& old) = delete;
    EditPartFactory(EditPartFactory& old) = delete;

protected:
    EditPartFactory(EditPolicyFactory& editPolicyFactory);

    EditPart* checkCache(const TTAMachine::MachinePart* component) const;
    void writeToCache(EditPart* editPart);

    /// Registered factories.
    std::vector<Factory*> factories_;
    /// Container for already created EditParts.
    static std::vector<EditPart*> created_;
    /// Factory which creates edit policies for edit parts.
    EditPolicyFactory& editPolicyFactory_;
};

#include "EditPartFactory.icc"

#endif
