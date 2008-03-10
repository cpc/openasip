/**
 * @file DataSectionCreator.hh
 *
 * Declaration of DataSectionCreator class.
 *
 * @author Mikael Lepistö 2005 (tmlepist@cs.tut.fi)
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
