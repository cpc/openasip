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
 * @file DataSectionCreator.hh
 *
 * Declaration of DataSectionCreator class.
 *
 * @author Mikael Lepistö 2005 (tmlepist-no.spam-cs.tut.fi)
 * @note rating: yellow
 */

#ifndef TCEASM_DATA_SECTION_CREATOR_HH
#define TCEASM_DATA_SECTION_CREATOR_HH

#include "Exception.hh"
#include "ParserStructs.hh"

namespace TPEF {
    class Binary;
    class Section;
    class DataSection;
}

class MachineResourceManager;
class LabelManager;
class Assembler;

/**
 * Collects data area information and parses data to tpef data sections.
 */
class DataSectionCreator {
public:
    DataSectionCreator(
        MachineResourceManager &resourceManager, Assembler* parent);

    ~DataSectionCreator();

    void setAreaStartAddress(UValue address);

    void addDataLine(const DataLine& origLine);

    void finalize(TPEF::Binary& tpef, LabelManager& labels, bool littleEndian)
        throw (CompileError);

    void cleanup();

private:
    /**
     * Internal representation of data section.
     */
    struct InternalSection {
        /// Address space of the section.
        std::string addressSpace;

        /// If section contains initialization data.
        bool isInitialized;
        /// Is start address of the section fixed.
        bool fixedStartAddress;

        /// The start address of the section.
        UValue startAddress;
        /// Number of MAUs stored in section.
        UValue length;

        /// Initialization data of the section. (or uninitialization data;)
        std::vector<DataLine*> lines;
    };

    void resolveDataAreaSizesAndLabelAddresses(
        std::map<std::string, UValue>& addressSpaceUsedMAUs,
        std::vector<std::string>& addedLabels,
        LabelManager &labels) throw (CompileError);

    UValue writeDataLineToTPEF(
        DataLine* line, TPEF::DataSection* dataSection,
        LabelManager& labels, UValue currentMAUIndex)
        throw (CompileError);

    int sizeInWords(UValue value, int MAUWidth, bool isSigned) const;

    /// TPEF Resources and strings.
    MachineResourceManager &resources_;

    /// Start address of next line (if fixed address).
    UValue currentStartAddress_;
    /// Is next data line address fixed.
    bool isStartAddressDefined_;

    /// Internal data sections.
    std::vector<InternalSection> sections_;
};



#endif
