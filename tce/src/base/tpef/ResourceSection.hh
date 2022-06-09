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
 * @file ResourceSection.hh
 *
 * Declaration of ResourceSection class.
 *
 * @author Mikael Lepistö 2004 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_RESOURCE_SECTION_HH
#define TTA_RESOURCE_SECTION_HH

#include "Section.hh"
#include "TPEFBaseType.hh"
#include "Exception.hh"
#include "ResourceElement.hh"

namespace TPEF {

/**
 * Prosessor Resource Table section.
 */
class ResourceSection : public Section {
public:
    virtual ~ResourceSection();

    virtual SectionType type() const;

    ResourceElement& findResource(
        ResourceElement::ResourceType aType, HalfWord anId) const;

    bool hasResource(
        ResourceElement::ResourceType aType, HalfWord anId) const;

protected:
    ResourceSection(bool init);
    virtual Section* clone() const;

private:
    /// Protorype instance of section.
    static ResourceSection proto_;
};
}

#endif
