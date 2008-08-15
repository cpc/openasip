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
 * @file ResourceSection.hh
 *
 * Declaration of ResourceSection class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
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
