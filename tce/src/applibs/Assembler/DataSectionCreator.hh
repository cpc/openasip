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

    void finalize(TPEF::Binary& tpef, LabelManager& labels)
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

    /// Parent assembler where to add warnings.
    Assembler* parent_;
};



#endif
