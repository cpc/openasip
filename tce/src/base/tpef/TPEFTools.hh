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
 * @file TPEFTools.hh
 *
 * Declaration of TPEFTools.
 *
 * @author Mikael Lepistö 2005 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_TOOLS_HH
#define TTA_TPEF_TOOLS_HH

#include <string>

#include "Application.hh"
#include "ResourceElement.hh"

namespace TPEF {

class Binary;
class ASpaceElement;
class SectionElement;
class RelocElement;
class Section;

/**
 * Contains many helper functions that makes TPEF handling
 * a bit easier.
 *
 * These are kind of functions that doesn't fit to any Sections' or 
 * SectionElements' interfaces. 
 *
 * Class can be used stright through static interface, 
 * or client may create instance of TPEFTools, which enables 
 * caching functionality. Basically, if TPEFTools is highly utilized
 * it should be used as a object.
 */
class TPEFTools {
public:
    TPEFTools(const Binary &aTpef);

    // clears cache tables
    void clearCache() const;

    Section& sectionOfElement(
        const SectionElement& element);

    std::string resourceName(
        ResourceElement::ResourceType type, 
        HalfWord resourceId) const;

    std::string addressSpaceName(
        const ASpaceElement& aSpace) const;

    bool hasRelocation(const SectionElement& element) const;

    const RelocElement& relocation(const SectionElement& element) const;

    static Section& sectionOfElement(
        const Binary& bin, 
        const SectionElement& element);

    static std::string resourceName(
        const Binary& bin,
        ResourceElement::ResourceType type,
        HalfWord resourceId);

    static std::string addressSpaceName(
        const Binary& bin,
        const ASpaceElement& aSpace);

    static bool hasRelocation(
        const Binary& bin,
        const SectionElement& element);

    static const RelocElement& relocation(
        const Binary& bin,
        const SectionElement& element);

private:
    /// TPEF whose resources are accessed.
    const Binary& tpef_;
    
    /// Cache of Chunks and immediates, which are relocated.
    mutable std::map<
        const SectionElement*, const RelocElement*>  relocationCache_;

    void initRelocationCache() const;

};

}

#endif
