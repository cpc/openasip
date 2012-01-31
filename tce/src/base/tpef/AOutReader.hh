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
 * @file AOutReader.hh
 *
 * Declaration of AOutReader.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 18.12.2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note reviewed 7 October 2003 by jn, ml, tr, ll
 * @note rating: yellow
 */

#ifndef TTA_AOUT_READER_HH
#define TTA_AOUT_READER_HH

#include "BinaryReader.hh"
#include "Binary.hh"
#include "TPEFBaseType.hh"
#include "Exception.hh"

namespace TPEF {
    class Binary;
    class CodeSection;
    class ResourceSection;
    class NullSection;
    class DebugSection;

/**
 * Reads binary file that is in TTA a.out format.
 */
class AOutReader : public BinaryReader {
public:

    /**
     * TODO: get these from tce-frontend
     * a.out symbols. (from MOVE/sched/exec.h)
     */
    enum SymbolType {
        N_UNDF  = 0x0,  ///< Undefined.
        N_ABS   = 0x2,  ///< Absolute.
        N_TEXT  = 0x4,  ///< Text.
        N_DATA  = 0x6,  ///< Data.
        N_BSS   = 0x8,  ///< BSS.
        N_COMM  = 0x12, ///< Common (internal to ld).
        N_FN    = 0x1e, ///< File name.
        N_EXT   = 0x1,  ///< Mask bit for external.
        N_TYPE  = 0x1e, ///< Mask bits for resolving symbol type.
        N_TJ    = 0x76, ///< Table jump.
        N_JTE   = 0x96, ///< Jump table entry.
        N_MEM   = 0xb6, ///< Memory reference.
        N_FC    = 0xd6, ///< Function call.
        N_STAB  = 0xe0, ///< Mask for stabs.
        N_GSYM  = 0x20, ///< Global.
        N_FNAME = 0x22, ///< Procedure name (f77).
        N_FUN   = 0x24, ///< Procedure.
        N_STSYM = 0x26, ///< Static.
        N_LCSYM = 0x28, ///< .lcomm.
        N_MAIN  = 0x2a, ///< Name of main rutine.
        N_RSYM  = 0x40, ///< Register.
        N_SLINE = 0x44, ///< Source line.
        N_SSYM  = 0x60, ///< Structure.
        N_SO    = 0x64, ///< Source file name.
        N_LSYM  = 0x80, ///< Local.
        N_BINCL = 0x82, ///< Header file.
        N_SOL   = 0x84, ///< #included file name.
        N_PSYM  = 0xa0, ///< Parameter.
        N_EINCL = 0xa2, ///< End of include file.
        N_LBRAC = 0xc0, ///< Left bracket.
        N_EXCL  = 0xc2, ///< Excluded include file.
        N_RBRAC = 0xe0, ///< Right bracket.
        N_BCOMM = 0xe2, ///< Begin common.
        N_ECOMM = 0xe4, ///< End common.
        N_ECOML = 0xe8, ///< End common (local name).
        N_LENG  = 0xfe, ///< Second symbol table entry with length.
        N_PC    = 0x30, ///< Global pascal symbol.
        N_M2C   = 0x42, ///< Compilation unit symbol table entry.
        N_SCOPE = 0xc4, ///< Scope information.
        N_BROWS = 0x48, ///< Sun source code browser.
        N_PRTAB = 0xd2, ///< TCE processors resource entry.
        N_ANN   = 0xa4  ///< TCE annotation entry.
    };


    /**
     * Names for a.out sections.
     *
     * Sections are expected in this order in a.out file.
     * Since there is only one section of each type, these names
     * can be used as section identifiers.
     */
    enum AOutSectionID {
        ST_UNDEF =  0x00,     ///< Undefined section.
        ST_TEXT =   N_TEXT,   ///< Text section.
        ST_DATA =   N_DATA,   ///< Data section.
        ST_UDATA =  N_BSS,    ///< Uninitialized data section.
        ST_SYMBOL = 0x10,     ///< Symbol table.
        ST_STRING = 0x12      ///< String table.
    };

    /// Number of integer registers.
    static const Word INT_REGISTERS;
    /// Number of floating-point registers.
    static const Word FP_REGISTERS;
    /// Number of Boolean registers.
    static const Word BOOL_REGISTERS;

    /// Index of the first integer register in a.out.
    static const Word FIRST_INT_REGISTER;
    /// Index of the first floating-point register.
    static const Word FIRST_FP_REGISTER;
    /// Index of the first Boolean register.
    static const Word FIRST_BOOL_REGISTER;
    /// Index of the first function unit register.
    static const Word FIRST_FU_REGISTER;

    /// Size of one instruction in a.out file.
    static const Byte AOUT_INSTRUCTION_SIZE;

    /// Minimum addressable word of address space.
    static const Byte AOUT_BITS_PER_MAU;
    /// Aligment of address space.
    static const Byte AOUT_WORD_ALIGN;
    /// Word size of address space.
    static const Byte AOUT_WORD_SIZE;
    
    /// Name of universal machines code address space.
    static const char* AOUT_CODE_ASPACE_NAME;
    /// Name of universal machines data address space.
    static const char* AOUT_DATA_ASPACE_NAME;    

    /**
     * Stores the data of a.out file header.
     */
    class Header {
    public:
        Header();
        virtual ~Header();

        void setSectionSizeData(Word size);
        void setSectionSizeUData(Word size);
        void setSectionSizeText(Word size);
        void setSectionSizeSymbol(Word size);
        void setSectionSizeTextReloc(Word size);
        void setSectionSizeDataReloc(Word size);
        void setSectionSizeString(Word size);
        Word sectionSizeData() const;
        Word sectionSizeUData() const;
        Word sectionSizeText() const;
        Word sectionSizeSymbol() const;
        Word sectionSizeTextReloc() const;
        Word sectionSizeDataReloc() const;
        Word sectionSizeString() const;

    private:

        /// This structure holds the sizes of every section in bytes.
        struct HeaderRawData {
            Word text_;       ///< The size of text section.
            Word data_;       ///< The size of data section.
            Word uData_;      ///< The size of uninitialized data section.
            Word textReloc_;  ///< The size of text relocation section.
            Word dataReloc_;  ///< The size of data relocation section.
            Word symbol_;     ///< The size of symbol table.
            Word string_;     ///< The size of string section.
        } sizes_;

    };

    static BinaryReader* instance();

    SectionOffset sectionOffsetOfAddress(AddressImage address) const
        throw (OutOfRange);

    AddressImage addressOfElement(SectionElement *elem) const
        throw (OutOfRange);

    ASpaceElement* aSpaceOfElement(SectionElement *elem) const
        throw (OutOfRange);

    ResourceSection* resourceTable() const;

    NullSection* nullSection() const;

    DebugSection* debugSection() const;

    StringSection* stringSection() const;

    CodeSection* textSection() const;

    static const Header& header();

protected:
    AOutReader();
    virtual ~AOutReader();

    virtual Binary* readData(BinaryStream& stream) const
        throw (InstanceNotFound, UnreachableStream, KeyAlreadyExists,
               EndOfFile, OutOfRange, WrongSubclass, UnexpectedValue);

    virtual bool isMyStreamType(BinaryStream& stream) const
        throw (UnreachableStream);

private:
    void readHeader(BinaryStream& stream) const
        throw (UnreachableStream, EndOfFile);

    void readSection(
        BinaryStream& stream,
        FileOffset startPosition,
        Section* section,
        Length length) const
        throw (InstanceNotFound, UnreachableStream, KeyAlreadyExists,
               EndOfFile, OutOfRange, WrongSubclass, UnexpectedValue);

    void addOrDeleteSection(Section* section, Binary* binary) const;

    /// Copying not allowed.
    AOutReader(const AOutReader&);
    /// Assignment not allowed.
    AOutReader& operator=(const AOutReader&);

    /// Undefined address space.
    mutable ASpaceElement* undefASpace_;
    /// Address space of text section.
    mutable ASpaceElement* codeASpace_;
    /// Address space of data sections.
    mutable ASpaceElement* dataASpace_;

    // Processors resource table for AOoutSymbolSectionReader
    static ResourceSection* resourceTable_;

    // Null section for AOoutSymbolSectionReader
    static NullSection* nullSection_;

    // Debug section
    static DebugSection* debugSection_;

    // String  section
    static StringSection* stringSection_;

    // Text section (a.out's code section)
    static CodeSection* textSection_;

    /// Move a.out format identifier.
    static const HalfWord OMAGIC;
    /// Size of file header.
    static const Byte FILE_HEADER_SIZE;

    /// Header of a.out file.
    static Header header_;
    /// Prototype instance of AOutReader registered into BinaryReader.
    static AOutReader* proto_;
};
}

#include "AOutReader.icc"
#endif
