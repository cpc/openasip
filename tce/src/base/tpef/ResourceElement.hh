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
 * @file ResourceElement.hh
 *
 * Declaration of ResourceElement class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_RESOURCE_ELEMENT_HH
#define TTA_RESOURCE_ELEMENT_HH

#include "TPEFBaseType.hh"
#include "SectionElement.hh"
#include "SafePointer.hh"
#include "Chunk.hh"

namespace TPEF {

/**
 * Resource table element.
 */
class ResourceElement : public SectionElement {
public:

    /// Resource types
    enum ResourceType {
        MRT_NULL = 0x00, ///< Illegal undefined machine resource.
        MRT_BUS  = 0x01, ///< Transport bus.
        MRT_UNIT = 0x02, ///< Function unit.
        MRT_RF   = 0x03, ///< Register file.
        MRT_OP   = 0x04, ///< Operation operand or function unit register.
        MRT_IMM  = 0x05, ///< Immediate unit.
        MRT_SR   = 0x06, ///< Special register.
        MRT_PORT = 0x07  ///< Function unit port.
    };

    /// Resource Id numbers that are reserved for some particular use.
    enum ReservedResourceIDs {
        UNIVERSAL_BUS     = 0x00, ///< Universal bus.
        UNIVERSAL_FU      = 0x00, ///< Universal function unit.
        ILLEGAL_RF        = 0x00, ///< Illegal register file.
        INT_RF            = 0x80, ///< Universal integer register file.
        BOOL_RF           = 0x81, ///< Universal boolean register file.
        FP_RF             = 0x82, ///< Universal floating point register file.
        UNIVERSAL_RF_MASK = 0x80, ///< Bit to check if registerfile is real or
                                  ///< universal.
        INLINE_IMM        = 0x00  ///< Inline immediate unit id.
    };

    /// Reserved string of return address special register
    static const std::string RETURN_ADDRESS_NAME;

    ResourceElement();
    virtual ~ResourceElement();

    HalfWord id() const;
    void setId(HalfWord aId);

    ResourceType type() const;
    void setType(ResourceType aType);

    Word info() const;
    void setInfo(Word aInfo);

    Chunk* name() const;
    void setName(ReferenceManager::SafePointer* aName);
    void setName(Chunk* aName);

private:
    /// Unique id  number that is used to refer resource.
    HalfWord id_;
    /// Type.
    ResourceType type_;
    /// Name.
    const ReferenceManager::SafePointer* name_;
    /// Additional info.
    Word info_;
};
}

#include "ResourceElement.icc"

#endif
