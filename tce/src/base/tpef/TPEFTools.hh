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
