/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file ResourceElement.hh
 *
 * Declaration of ResourceElement class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
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
