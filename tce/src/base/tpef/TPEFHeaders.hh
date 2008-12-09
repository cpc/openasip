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
 * @file TPEFHeaders.hh
 *
 * TPEF file and section header related offsets and
 * some other TPEF binary file related values.
 *
 * @author Mikael Lepistö 2004 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_HEADERS
#define TTA_TPEF_HEADERS

#include "TPEFBaseType.hh"

namespace TPEF {
namespace TPEFHeaders {

    /// Magic number.
    const Byte FH_ID_BYTES[] = {
        0x7f, 0x54, 0x54, 0x41, 0x2d, 0x50, 0x46, 0x00, 0x01, 0x0a
    };

    /// Size of file identification code.
    const Byte     FH_ID_SIZE     = 10;
    /// Size of file header.
    const HalfWord FH_HEADER_SIZE = 26;
    /// Suze of section header.
    const HalfWord SH_HEADER_SIZE = 32;

    /**
     * Offsets of file header.
     */
    enum FileHeaderOffset {
        FH_ID       = 0,  ///< File identification code.
        FH_ARCH     = 10, ///< Architecture template.
        FH_TYPE     = 11, ///< Type of TTA program.
        FH_SHOFF    = 12, ///< Offset to first section header.
        FH_SIZE     = 16, ///< File header size.
        FH_SHSIZE   = 18, ///< Size of section header entry.
        FH_SHNUM    = 20, ///< Number of section headers.
        FH_SHSTRTAB = 22  ///< Offset to header of string table.
    };

    /**
     * Offsets of section header.
     */
    enum SectionHeaderOffset {
        SH_NAME    = 0,  ///< Section offset to name.
        SH_TYPE    = 4,  ///< Type of section.
        SH_FLAGS   = 5,  ///< Flags of section.
        SH_ADDR    = 6,  ///< Starting memory address of program section.
        SH_OFFSET  = 10, ///< Offset to section data.
        SH_SIZE    = 14, ///< Size of section data.
        SH_ID      = 18, ///< Section identification code.
        SH_ASPACE  = 20, ///< Section address space identifier.
        SH_PADDING = 21, ///< Padding, must be zero.
        SH_LINK    = 22, ///< Section identifier link.
        SH_INFO    = 24, ///< Section specific information, usually zero.
        SH_ENTSIZE = 28  ///< Size of section elements (if fixed size).
    };

    /**
     * Values for SymbolElement other field.
     */
    enum SymbolOtherValues {
        STO_ABS = 0x80 ///< Section is absolute, not relocating.
    };

    /**
     * Values for RelocationElement type field
     */
    enum RelocTypeValues {
        STF_RELOCATION_TYPE_MASK = 0x0F, ///< Mask for getting reloc type.
        STF_CHUNK = 0x80                 ///< Relocation applied to chunk(1)
                                         ///< or complete address(0).
    };

    /**
     * InstructionAnnotation flags and masks.
     */
    enum InstructionAnnotationMasks {
        IANNOTE_CONTINUATION = 0x80, ///< If there is more annotations.
        IANNOTE_SIZE         = 0x7f  ///< Size of payload of annotation.
    };

    /**
     * Instruction attribute flags and masks.
     */
    enum InstructionAttributeField {
        IA_TYPE    = 0x01, ///< Instruction type: move (0), immediate (1).
        IA_END     = 0x02, ///< Is end of instruction.
        IA_ANNOTE  = 0x04, ///< Contains annotation.
        IA_EMPTY   = 0x08, ///< Empty instruction.
        IA_IMMSIZE = 0xf0, ///< Immediade size mask.
        IA_MGUARD  = 0x10  ///< Is conditional move or unconditional move.
    };

    /**
     * Masks for getting source and destination fields from instruction.
     */
    enum InstructionFieldType {
        IE_SRC_TYPE_MASK  = 0x0c, ///< Instruction source type mask.
        IE_DST_TYPE_MASK  = 0x30, ///< Instruction destination type mask.
        IE_GUARD_TYPE_MASK = 0x80,///< If (1) guard points to GPR,(0) to FU.
        MVS_NULL    = 0x00, ///< Illegal source.
        MVS_RF      = 0x04, ///< Source is RF.
        MVS_IMM     = 0x08, ///< Source is immediate.
        MVS_UNIT    = 0x0c, ///< Source is FU.
        MVD_NULL    = 0x00, ///< Illegal destination.
        MVD_RF      = 0x10, ///< Destination is RF.
        MVD_ILLEGAL = 0x20, ///< Illegal destination.
        MVD_UNIT    = 0x30, ///< Destination is FU.
        MVG_UNIT    = 0x00, ///< Guard is FU.
        MVG_RF      = 0x80, ///< Guard is RF.
        IE_GUARD_INV_MASK = 0x40 ///< Guard inverted (1) means inverted.
    };

}   // namespace TPEFHeaders;
}   // namespace TPEF;

#endif
