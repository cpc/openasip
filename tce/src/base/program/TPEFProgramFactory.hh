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
 * @file TPEFProgramFactory.hh
 *
 * Declaration of TPEFProgramFactory class.
 *
 * @author Mikael Lepistö 2005 (tmlepist@cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pjaaskel@cs.tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_TPEF_PROGRAM_FACTORY_HH
#define TTA_TPEF_PROGRAM_FACTORY_HH

#include <map>
#include <list>

#include "Binary.hh"
#include "Machine.hh"
#include "Program.hh"
#include "UniversalMachine.hh"
#include "MoveElement.hh"
#include "ImmediateElement.hh"
#include "Terminal.hh"
#include "ResourceElement.hh"
#include "ResourceSection.hh"
#include "Socket.hh"
#include "TPEFTools.hh"
#include "AddressSpace.hh"
#include "ASpaceElement.hh"

namespace TTAProgram {

class Move;
class Immediate;

/**
 * Creates a new TTA program out of a description of a binary representation
 * of the program and a model of the target architecture.
 *
 * This class builds only one program, even if the input binary contains many
 * programs (the original TPEF file has multiple code sections with different
 * address spaces).
 */
class TPEFProgramFactory {

public:
    TPEFProgramFactory(
        const TPEF::Binary &aBinary,
        const TTAMachine::Machine &aMachine,
        const UniversalMachine &aUniversalMachine);
    
    TPEFProgramFactory(
        const TPEF::Binary &aBinary,
        const TTAMachine::Machine &aMachine);
    
    virtual ~TPEFProgramFactory();

    Program* build() throw (NotAvailable, Exception);
    
protected:
   
    typedef std::map<HalfWord, SimValue*> InlineValues;    
    typedef std::pair<Word, Word> ImmediateKey;
    typedef std::map<ImmediateKey, TPEF::ImmediateElement*> ImmediateMap;
    typedef std::vector<TPEF::MoveElement*> MoveVector;
    typedef std::vector<TPEF::ImmediateElement*> ImmediateVector;
    
    Instruction* createInstruction(
        const TPEF::ResourceSection &resources,
        MoveVector& moveElements,
        ImmediateVector& longImmediates,
        ImmediateMap& immElements) const;
    
    
    void addProcedures(
        Program &program,
        const TTAMachine::AddressSpace &programASpace) const;
    
    Terminal* createTerminal(
        const TPEF::ResourceSection &resources,
        const TTAMachine::Bus &aBus,
        TTAMachine::Socket::Direction direction,
        TPEF::MoveElement::FieldType type,
        Byte unitId,
        HalfWord index,
        const ImmediateMap *immediateMap = NULL) const;
    
    TTAMachine::Bus& findBus(
        const TPEF::ResourceSection &resources,
        Byte busId) const;
    
    TTAMachine::RegisterFile& findRegisterFile(
        const TPEF::ResourceSection &resources,
        Byte rfId) const;
    
    TTAMachine::ImmediateUnit& findImmediateUnit(
        const TPEF::ResourceSection &resources,
        Byte immUnitId) const;
    
    TTAMachine::FunctionUnit& findFunctionUnit(
        const TPEF::ResourceSection &resources,
        Byte unitId, std::string tpefOpName = "") const;
    
    TTAMachine::Port& findPort(
        const TTAMachine::Bus &bus,
        const TTAMachine::Unit &portParent,
        std::string tpefOpName = "",
        int tpefOpIndex = 0) const;

    TTAMachine::AddressSpace& findAddressSpace(
        const TPEF::ASpaceElement *aSpace) const;
    
    TTAMachine::Guard& findGuard(
        const TPEF::ResourceSection &resources,
        TTAMachine::Bus &bus,
        TPEF::MoveElement::FieldType type,
        Byte unitId,
        HalfWord index,
        bool isInverted) const;
    
    TTAMachine::InstructionTemplate& findInstrTemplate(
        const TPEF::ResourceSection &resources,
        ImmediateVector& longImmediates,
        MoveVector& moves) const;
    
    void seekFunctionStartPoints();

    void createLabels(Program &prog);
    
    void createDataRelocs(Program &prog);

    void createDataMemories(Program &prog);
    
    bool isFunctionStart(
        const TPEF::InstructionElement &instructionElement) const;
    
    std::string functionName(
        const TPEF::InstructionElement &instructionElement) const;
    
    std::string stringOfChunk(
        const TPEF::Chunk *chunk, const TPEF::Section *chunkOwner) const;
    
    
private:
    
    /// Cache key for resources that are accessed from MOM(s)
    class CacheKey {
    public:
        CacheKey(
            const TTAMachine::Bus &aBus,
            TTAMachine::Socket::Direction aDirection,
            TPEF::MoveElement::FieldType aType,
            Byte anUnitId, HalfWord anIndex) :
            bus_(aBus), direction_(aDirection), type_(aType),
            unitId_(anUnitId), index_(anIndex) { }
               
        bool operator<(const CacheKey &keyToCompare) const {
            
            if (direction_ < keyToCompare.direction_) return true;
            else if (direction_ > keyToCompare.direction_) return false;
            
            if (type_ < keyToCompare.type_) return true;
            else if (type_ > keyToCompare.type_) return false;
            
            if (unitId_ < keyToCompare.unitId_ ) return true;
            else if (unitId_ > keyToCompare.unitId_ ) return false;
            
            if (index_ < keyToCompare.index_ ) return true;
            else if (index_ > keyToCompare.index_) return false;
            
            if (&bus_ < &(keyToCompare.bus_)) return true;
            
            return false;
        }
        
    private:
        /// Bus that was used for transport.
        const TTAMachine::Bus& bus_;
        /// Direction if the port was read or written.
        TTAMachine::Socket::Direction direction_;
        /// Type of the accessed unit.
        TPEF::MoveElement::FieldType type_;
        /// unit id of the corresponding TPEF resource element.
        Byte unitId_;
        /// Index of the corresponding TPEF resource element.
        HalfWord index_;
    };
    
    /**
     * Helper struct for resolving socket allocations
     * for register file access.
     *
     * This could be removed, if register file port storing for register
     * references is implemented in TPEF.
     */
    struct SocketAllocation {
        SocketAllocation(Move* m, unsigned int anIndex) :
            index(anIndex) ,move(m), src(0), dst(0) { };

        unsigned int index;
        Move* move;
        std::vector<TTAMachine::Socket*> srcSocks;
        std::vector<TTAMachine::Socket*> dstSocks;
        unsigned int src;
        unsigned int dst;
    };
    
    void clearResourceAllocations() const;
    
    bool canSourceBeAssigned(
        SocketAllocation& alloc,
        std::map<TTAMachine::Socket*, 
        std::vector<SocketAllocation*> >& fixedSockets) const;
    
    void resolveSocketAllocations(
        std::vector<SocketAllocation>& allocs) const;
    
    Terminal* getFromCache(const CacheKey &key) const;
    void addToCache(const CacheKey &key, Terminal *cachedTerm) const;
    void clearCache() const;
    
    /// Binary that is used for creating program.
    const TPEF::Binary* binary_;
    /// Target machine of program.
    const TTAMachine::Machine* machine_;
    /// Universal machine of program.
    const UniversalMachine* universalMachine_;

    /// TPEFTools object for helper functions.
    TPEF::TPEFTools tpefTools_;
    /// Instruction address space of machine.
    TTAMachine::AddressSpace* adfInstrASpace_;
    /// Instruction address space element of TPEF.
    TPEF::ASpaceElement* tpefInstrASpace_;
    
    /// Stores information of start points of procedures that were found.
    std::map<const TPEF::InstructionElement*,
             class FunctionStart*> functionStartPositions_;
    
    /// Moves whose source terminals are addresses referring to instructions.
    mutable std::list<Move*> instructionImmediates_;
    
    /// Long immediates whose value terminals refers to instructions.
    mutable std::list<Immediate*> longInstructionImmediates_;
    
    /// Cache map of terminals that are returned by different search 
    /// parameters.
    mutable std::map<const CacheKey, Terminal *> cache_;
    
    /// Busses that are already allocated by moves of current instruction.
    mutable std::set<Byte> allocatedBusses_;
    
    /// Program instruction by TPEF instruction element.
    mutable std::map<TPEF::InstructionElement*, Instruction*> instructionMap_;

};

}

#endif
