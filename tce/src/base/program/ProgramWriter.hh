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
 * @file ProgramWriter.hh
 *
 * Declaration of ProgramWriter class.
 *
 * @author Mikael Lepistö 2005 (tmlepist@cs.tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_PROGRAM_WRITER_HH
#define TTA_PROGRAM_WRITER_HH

#include <map>

#include "BaseType.hh"
#include "MoveElement.hh"
#include "Binary.hh"
#include "Address.hh"

namespace TPEF {
    class Binary;
    class CodeSection;
    class RelocSection;
    class SymbolSection;
    class ResourceSection;
    class ResourceElement;
    class Section;
    class ASpaceElement;
    class SectionElement;
}

namespace TTAMachine {
    class Machine;
    class Bus;
    class RegisterFile;
    class ImmediateUnit;
    class FunctionUnit;
    class Port;
    class HWOperation;
    class MachinePart;
    class AddressSpace;
}

namespace TTAProgram {

class Program;
class Terminal;

/**
 * Returns requested Machine parts as a TPEF resources.
 *
 * Also adds new resources to resource section if resource is not
 * already there.
 *
 * NOTE: If resource section is modified externally, cache should be cleared.
 *
 */
class TPEFResourceUpdater {
public:
    TPEFResourceUpdater(
        TTAMachine::Machine& mach,
        TPEF::ResourceSection& resources);

    void clearCache();

    TPEF::ResourceElement& bus(const TTAMachine::Bus& bus);
    TPEF::ResourceElement& functionUnitPort(const TTAMachine::Port& port);
    TPEF::ResourceElement& operand(
        const TTAMachine::HWOperation& oper,
        int operandIndex);
    TPEF::ResourceElement& functionUnit(const TTAMachine::FunctionUnit& fu);
    TPEF::ResourceElement& registerFile(const TTAMachine::RegisterFile& rf);
    TPEF::ResourceElement& immediateUnit(
        const TTAMachine::ImmediateUnit& immUnit);

private:
    void initCache();

    /// The model of the target processor architecture.
    TTAMachine::Machine& mach_;
    /// The TPEF input section that contains all machine resource entries.
    TPEF::ResourceSection& resources_;
    
    // Key type for caching ResourceElements for machine components.
    // For example (HWOperation, index) pair
    typedef std::pair<const TTAMachine::MachinePart*, int> CacheKey;
    /// Mapping from parts of the target processor model to TPEF machine
    /// resource entries.
    typedef std::map<CacheKey, TPEF::ResourceElement*> ResourceCache;

    /// Aggregate of all mappings between machine resource entries and
    /// target processor parts (machine parts).
    ResourceCache cache_;

    /// Last TPEF bus id that was generated.
    HalfWord lastBusId_;
    /// Last TPEF unit port id that was generated.
    HalfWord lastUnitPortId_;
    /// Last TPEF function unit id that was generated.
    HalfWord lastFunctionUnitId_;
    /// Last TPEF register file id that was generated.
    HalfWord lastRegisterFileId_;
    /// Last TPEF immediate unit id that was generated.
    HalfWord lastImmediateUnitId_;
};

/**
 * Writes new TPEF out of POM and original TPEF.
 *
 * Writer creates creates whole new code section, code relocation section,
 * resource section and symbol section for code symbols. Data sections,
 * uninitialized data sections and data to data relocations are also copied
 * from original TPEF.
 *
 * Client of POM has to write data to code relocations and all sections
 * that client wants to preserve from original TPEF.
 */
class ProgramWriter {
public:
    ProgramWriter(const Program& prog);

    TPEF::Binary* createBinary() const
        throw (Exception);

private:
    /**
     * TPEF resource id:s for getting corresponding resource elements.
     */
    struct ResourceID {
        TPEF::MoveElement::FieldType type; ///< RF, UNIT or IMM.
        HalfWord unit; ///< Unit id.
        HalfWord index; ///< Operand or register file index.
    };
    
    /**
     * Info about relocation of immediate or data chunk.
     */
    struct RelocInfo {
        RelocInfo(
            TPEF::Section* aSrcSect, TPEF::SectionElement* aSrcElem, 
            const Address& aDstAddress, int bitWidth) : 
            srcSect(aSrcSect), srcElem(aSrcElem),
            destination(aDstAddress), bits(bitWidth) {
        }
        
        /// Location section.
        TPEF::Section* srcSect;
        /// Location element.
        TPEF::SectionElement* srcElem;
        /// Destination address.
        Address destination;
        /// Relocation width.
        int bits;      
    };

    void createCodeSection(
        TPEF::CodeSection* code,
        TPEFResourceUpdater& updater) const;

    void createDataSections(TPEF::Binary* bin) const;

    void createRelocSections(TPEF::Binary* bin) const
        throw (NotAvailable);

    TPEF::Section& findSection(TPEF::Binary& bin, Address address) const;

    ResourceID terminalResource(
        const Terminal& term,
        TPEFResourceUpdater& updater) const;

    TPEF::Binary::FileType resolveFileType(
        TPEF::ResourceSection& resources) const;
    
    TPEF::ASpaceElement& createASpaceElement(
        const TTAMachine::AddressSpace& addressSpace, 
        TPEF::Binary& bin) const;

    
    /// Program that is written to TPEF.
    const Program& prog_;
    /// Created TPEF binary.
    const TPEF::Binary& tpef_;

    /// Map that contains created address spaces
    mutable std::map<const TTAMachine::AddressSpace*, 
                     TPEF::ASpaceElement*> aSpaceMap_;

    mutable std::vector<RelocInfo> relocInfos_;
    
    /// Default widt that is used as address width of relocations of immediate
    /// elements. This should be fixed by checking width from bus or template.
    static const HalfWord IMMEDIATE_ADDRESS_WIDTH;
};

}

#endif
