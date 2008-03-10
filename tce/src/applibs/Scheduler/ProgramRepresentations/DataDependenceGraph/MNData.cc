/**
 * @file MNData.cc
 *
 * Implementation of MNData class which is a class helping DDG construction.
 * MNData class is used as wrapper around MoveNode during 
 * DataDependenceGraph construction. MNData adds operations to easily 
 * check dependencies between moves etc.
 *
 * @author Heikki Kultala 2008 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#include "MNData.hh"

#include <list>

#include "TerminalRegister.hh"
#include "RegisterFile.hh"
#include "MoveNode.hh"
#include "Guard.hh"
#include "MoveGuard.hh"
#include "BaseFUPort.hh"
#include "Operation.hh"

using namespace TTAProgram;
using namespace TTAMachine;

using std::list;

static const int REG_SP = 1;
static const int REG_RV = 0;
static const int REG_IPARAM = 2;

class DataDependenceGraph;
class BasicBlockNode;


/** 
 * Constructs a MNData.
 * 
 * @param mn MoveNode to analyze
 * @param regs map containing special registers.
 * @param entryNode whether node is an entry node and not actual move
 */
MNData::MNData(
    MoveNode* mn, std::map<int,TTAProgram::TerminalRegister*>& registers, 
    bool entryNode) 
    : moveNode_(mn) {
    
    if (entryNode) {
        for (int i = REG_SP; i < REG_IPARAM+4; i++) {
            std::map<int,TTAProgram::TerminalRegister*>::iterator iter =
                registers.find(i);
            if (iter != registers.end()) {
                regWrites_.push_back(
                    dynamic_cast<TerminalRegister*>(iter->second->copy()));
            }
        }
    } else {

        Terminal& src = mn->move().source();
        if (src.isGPR()) {
            regReads_.push_back(dynamic_cast<TerminalRegister*>(src.copy()));
        }

        // guard usage
        if (!mn->move().isUnconditional()) {
            Guard& g = mn->move().guard().guard();
            RegisterGuard* rg = dynamic_cast<RegisterGuard*>(&g);
            if (rg != NULL) {
                RegisterFile* grFile = rg->registerFile();
                int grIndex = rg->registerIndex();
                guardRegReads_.push_back(
                    new TerminalRegister(*(grFile->port(0)), grIndex));
                
            } else { 
                // TODO: what to do with port guards?
            }
        }

        if (isCall()) {
            std::map<int,TTAProgram::TerminalRegister*>::iterator iter =
                registers.find(0);
            if (iter != registers.end()) {
                pseudoRegWrites_.push_back(
                    dynamic_cast<TerminalRegister*>(iter->second->copy()));
            }
            
            iter = registers.find(1);
            if (iter != registers.end()) {
                pseudoRegReads_.push_back(
                    dynamic_cast<TerminalRegister*>(iter->second->copy()));
            }

            for (int i = REG_IPARAM; i < REG_IPARAM+4; i++) {
                std::map<int,TTAProgram::TerminalRegister*>::iterator iter =
                    registers.find(i);
                if (iter != registers.end()) {
                    pseudoRegReads_.push_back(
                        dynamic_cast<TerminalRegister*>(iter->second->copy()));
                }
            }
        } else {
            if (isReturn()) {
                for (int i = REG_RV; i < REG_IPARAM; i++) {
                    std::map<int,TTAProgram::TerminalRegister*>::iterator 
                        iter = registers.find(i);
                    if (iter != registers.end()) {
                        pseudoRegReads_.push_back(
                            dynamic_cast<TerminalRegister*>(
                                iter->second->copy()));
                    }
                }
                
            } else {
                // ordinary write
                Terminal& dest = mn->move().destination();
                if (dest.isGPR()) {
                    regWrites_.push_back(
                        dynamic_cast<TerminalRegister*>(dest.copy()));
                }
            }
        }
    }
}

/**
 * Copy constructor. 
 * Copies everything from old MNData to new.
 *
 * @param mnData old MNData.
 */
MNData::MNData(const MNData& mnData) :
    moveNode_(mnData.moveNode_), regWrites_(mnData.regWrites_.size()),
    regReads_(mnData.regReads_.size()), 
    pseudoRegWrites_(mnData.pseudoRegWrites_.size()),
    pseudoRegReads_(mnData.pseudoRegReads_.size()) {

    for (unsigned int i = 0 ; i < mnData.regWrites_.size(); i++) {
        regWrites_[i] =
            dynamic_cast<TerminalRegister*>(mnData.regWrites_[i]->copy());
    }

    for (unsigned int i = 0 ; i < mnData.pseudoRegWrites_.size(); i++) {
        pseudoRegWrites_[i] =
            dynamic_cast<TerminalRegister*>(
                mnData.pseudoRegWrites_[i]->copy());
    }

    for (unsigned int i = 0 ; i < mnData.regReads_.size(); i++) {
        regReads_[i] =
            dynamic_cast<TerminalRegister*>(mnData.regReads_[i]->copy());
    }

    for (unsigned int i = 0 ; i < mnData.pseudoRegReads_.size(); i++) {
        pseudoRegReads_[i] =
            dynamic_cast<TerminalRegister*>(mnData.pseudoRegReads_[i]->copy());
    }
}

/**
 * Destructor.
 * Deletes all register read and write data from this MNData.
 */
MNData::~MNData() {
    for (unsigned int i = 0; i < regReads_.size(); i++) {
        delete regReads_[i];
    }
    regReads_.clear();

    for (unsigned int i = 0; i < pseudoRegReads_.size(); i++) {
        delete pseudoRegReads_[i];
    }
    pseudoRegReads_.clear();

    for (unsigned int i = 0; i < regWrites_.size(); i++) {
        delete regWrites_[i];
    }
    regWrites_.clear();

    for (unsigned int i = 0; i < pseudoRegWrites_.size(); i++) {
        delete pseudoRegWrites_[i];
    }
    pseudoRegWrites_.clear();
}

/**
 * Checks if the move related to this MNData triggers a call operation.
 * 
 * Just simple wrapper to move's iscall routine.
 *
 * @return true if this move triggers a call operation.
 */
bool MNData::isCall() const {
    return (moveNode_->isMove() && moveNode_->move().isCall());
}

/// @todo This should be moved to POM
/**
 * Checks if the move related to this MNData triggers a 
 * return from procedure.
 * 
 * @return true if this move triggers a return.
 */
bool 
MNData::isReturn() const {
    TTAProgram::Move& move = moveNode_->move();
    if (move.destination().isFUPort()) {
        const BaseFUPort& dst1 =
            dynamic_cast<const BaseFUPort&>(move.destination().port());
        if (dst1.isTriggering()) {
            const TerminalFUPort& tfup =
                dynamic_cast<const TerminalFUPort&>(move.destination());
            std::string opName = tfup.operation().name();
            if (opName == "JUMP" && move.source().isFUPort())
                return true;
        }
    }
    return false;
}

/**
 * Gives the movenode this MNData relates to
 *
 * @return the movenode 
 */
MoveNode& 
MNData::node() const {
    return *moveNode_;
}

/**
 * Checks if a RAW dependence exists between moves
 *
 * @param prev MNData related to previous move
 * @return true if prev writes to same register than this reads from.
 */
bool 
MNData::isRaw(const MNData& prev) const {
    for (unsigned int i = 0; i < regReads_.size(); i++) {
        if (prev.writesTo(*regReads_[i]) || 
            prev.pseudoWritesTo(*regReads_[i])) {
            return true;
        }
    }
    return false;
}

/**
 * Checks if a pseudo-RAW dependence exists between moves
 *
 * Checks if later move has a pseudo-read ( ie parameter read of a function)
 * to a variable the earlier writes to.
 * 
 * @param prev MNData related to previous move
 * @return true if prev writes to same register than this reads from.
 */
bool
MNData::isRawToPseudo(const MNData& prev) const {
    for (unsigned int i = 0; i < pseudoRegReads_.size(); i++) {
        if (prev.writesTo(*pseudoRegReads_[i]) || 
            prev.pseudoWritesTo(*pseudoRegReads_[i])) {
            return true;
        }
    }
    return false;
}

/**
 * Checks if a RAW dependence exists between moves, by guard usage
 *
 * @param prev MNData related to previous move
 * @return true if prev writes to same register than this uses as guard.
 */
bool
MNData::isRawToGuard(const MNData& prev) const {
    for (unsigned int i = 0; i < guardRegReads_.size(); i++) {
        if (prev.writesTo(*guardRegReads_[i]) || 
            prev.pseudoWritesTo(*guardRegReads_[i])) {
            return true;
        }
    }
    return false;
}


/**
 * Checks if a WAW antidependence exists between moves
 *
 * @param prev MNData related to previous move
 * @return true if prev writes to same register than this writes to.
 */
bool 
MNData::isWaw(const MNData& prev) const {
    for (unsigned int i = 0; i < regWrites_.size(); i++) {
        if (prev.writesTo(*regWrites_[i])||
            prev.pseudoWritesTo(*regWrites_[i])) {
            return true;
        }
    }

    for (unsigned int i = 0; i < pseudoRegWrites_.size(); i++) {
        if (prev.writesTo(*pseudoRegWrites_[i])||
            prev.pseudoWritesTo(*pseudoRegWrites_[i])) {
            return true;
        }
    }

    return false;
}

/**
 * Checks if a WAR antidependence exists between moves
 *
 * @param prev MNData related to previous move
 * @return true if prev reads from same register than this writes to.
 */
bool 
MNData::isWar(const MNData& prev) const {

    // prevent dependencies to itself, ie MNData with same movenode
    if( &(this->node()) == &(prev.node()) ) {
        return false;
    }

    for (unsigned int i = 0; i < regWrites_.size(); i++) {
        if (prev.readsFrom(*regWrites_[i])||
            prev.pseudoReadsFrom(*regWrites_[i])||
            prev.guardReadsFrom(*regWrites_[i])) {
            return true;
        }
    }
    for (unsigned int i = 0; i < pseudoRegWrites_.size(); i++) {
        if (prev.readsFrom(*pseudoRegWrites_[i])||
            prev.pseudoReadsFrom(*pseudoRegWrites_[i])||
            prev.guardReadsFrom(*pseudoRegWrites_[i])) {
            return true;
        }
    }

    return false;
}


/**
 * Checks whether the move related to this MNData reads 
 * from a specified register.
 * This does not include pseudu reads, ie. function parameter reads
 * by function calls etc.
 *
 * @param tr The register to check for reads.
 * @return returns true if this move reads from given register
 */
bool 
MNData::readsFrom(
    const TTAProgram::TerminalRegister& tr) const {
    return readsFrom(tr.registerFile(), tr.index());
}

/**
 * Checks whether the move related to this MNData pseudo-reads 
 * from a specified register.
 *
 * This means ie. function parameter reads by function calls etc.
 *
 * @param tr The register to check for reads.
 * @return returns true if this move reads from given register
 */
bool 
MNData::pseudoReadsFrom(
    const TTAProgram::TerminalRegister& tr) const {
    return pseudoReadsFrom(tr.registerFile(), tr.index());
}

/**
 * Checks whether the move related to this MNData uses guard
 * from a specified register.
 *
 * @param tr The register to check for guard usage
 * @return returns true if this move reads from given register
 */
bool 
MNData::guardReadsFrom(
    const TTAProgram::TerminalRegister& tr) const {
    return guardReadsFrom(tr.registerFile(), tr.index());
}

/**
 * Checks whether the move related to this MNData writes 
 * to a specified register.
 *
 * This does not include pseudo-writes ie. function call return values etc.
 *
 * @param tr The register to check for writes
 * @return returns true if this move writes to the given register
 */
bool 
MNData::writesTo(
    const TTAProgram::TerminalRegister& tr) const {
    return writesTo(tr.registerFile(), tr.index());
}

/**
 * Checks whether the move related to this MNData pseudo-writes 
 * to a specified register.
 *
 * This means return value write by function calls etc.
 *
 * @param tr The register to check for writes
 * @return returns true if this move writes to the given register
 */
bool 
MNData::pseudoWritesTo(
    const TTAProgram::TerminalRegister& tr) const {
    return pseudoWritesTo(tr.registerFile(), tr.index());
}

/**
 * Checks whether the move related to this MNData reads 
 * from a specified register.
 * This does not include pseudu reads, ie. function parameter reads
 * by function calls etc.
 *
 * @param rf Register file of the register
 * @param index Register index in register file of the Register
 * @return returns true if this move reads from given register
 */
bool 
MNData::readsFrom(
    const TTAMachine::RegisterFile& rf, unsigned int index) const {
    for (unsigned int i = 0; i < regReads_.size(); i++) {
        if (regReads_[i]->index() == static_cast<int>(index) &&
            &regReads_[i]->registerFile() == &rf) {
            return true;
        }
    }
    return false;
}

/**
 * Checks whether the move related to this MNData pseudo-reads 
 * from a specified register. This means function parameter reads
 * by function calls etc.
 *
 * @param rf Register file of the register
 * @param index Register index in register file of the Register
 * @return returns true if this move reads from given register
 */
bool 
MNData::pseudoReadsFrom(
    const TTAMachine::RegisterFile& rf, unsigned int index) const {
    for (unsigned int i = 0; i < pseudoRegReads_.size(); i++) {
        if (pseudoRegReads_[i]->index() == static_cast<int>(index) &&
            &pseudoRegReads_[i]->registerFile() == &rf) {
            return true;
        }
    }
    return false;
}

/**
 * Checks whether the move related to this MNData uses guard
 * from a specified register. 
 *
 * @param rf Register file of the register
 * @param index Register index in register file of the Register
 * @return returns true if this move reads from given register
 */
bool 
MNData::guardReadsFrom(
    const TTAMachine::RegisterFile& rf, unsigned int index) const {
    for (unsigned int i = 0; i < guardRegReads_.size(); i++) {
        if (guardRegReads_[i]->index() == static_cast<int>(index) &&
            &guardRegReads_[i]->registerFile() == &rf) {
            return true;
        }
    }
    return false;
}


/**
 * Checks whether the move related to this MNData writes 
 * to a specified register.
 * This does not include also pseudu writes, 
 * ie. return value write by function calls etc.
 *
 * @param rf Register file of the register
 * @param index Register index in register file of the Register
 * @return returns true if this move writes to the given register
 */
bool 
MNData::writesTo(
    const TTAMachine::RegisterFile& rf,
    unsigned int index) const {
    for (unsigned int i = 0; i < regWrites_.size(); i++) {
        if (regWrites_[i]->index() == static_cast<int>(index) &&
            &regWrites_[i]->registerFile() == &rf) {
            return true;
        }
    }
    return false;
}

/**
 * Checks whether the move related to this MNData pseudo-writes 
 * to a specified register.
 * This also means return value write by function calls etc.
 *
 * @param rf Register file of the register
 * @param index Register index in register file of the Register
 * @return returns true if this move writes to the given register
 */
bool 
MNData::pseudoWritesTo(
    const TTAMachine::RegisterFile& rf,
    unsigned int index) const {
    for (unsigned int i = 0; i < pseudoRegWrites_.size(); i++) {
        if (pseudoRegWrites_[i]->index() == static_cast<int>(index) &&
            &pseudoRegWrites_[i]->registerFile() == &rf) {
            return true;
        }
    }
    return false;
}

/**
 * Returns the number of registers this move directly or 
 * indirectly(via function call etc.) writes to
 * 
 * @return number of registers written by this move
 
int 
DataDependenceGraphBuilder::MNData::regWriteCount() const {
    return regWrites_.size() ;
}
*/

/* 
 * Removes a register write from MNData bookkeeping.
 *
 * @param rf RegisterFile of register being removed from write bookkeeping.
 * @param index register index of register being removed from write bookkeeping
 *
 * @return The number of writes(and preudo-writes) 
 * by the move after the removal.
 */

int 
MNData::removeWrite(
    const TTAMachine::RegisterFile& rf,
    unsigned int index) {
    for (std::vector<TerminalRegister*>::iterator iter =
             regWrites_.begin(); iter != regWrites_.end(); iter++) {
        if ((*iter)->index() == static_cast<int>(index) &&
            &(*iter)->registerFile() == &rf) {
            delete *iter;
            regWrites_.erase(iter);
            return pseudoRegWrites_.size() + regWrites_.size();
        }
    }

    for (std::vector<TerminalRegister*>::iterator iter =
             pseudoRegWrites_.begin(); 
         iter != pseudoRegWrites_.end(); iter++) {
        if ((*iter)->index() == static_cast<int>(index) &&
            &(*iter)->registerFile() == &rf) {
            delete *iter;
            pseudoRegWrites_.erase(iter);
            return pseudoRegWrites_.size() + regWrites_.size();
        }
    }

    return pseudoRegWrites_.size() + regWrites_.size();
}

/** 
 * Removes a register read from MNData bookkeeping.
 *
 * List of reads is kept for WaR bookkeeping.
 *
 * @param rf RegisterFile of register being removed from read bookkeeping.
 * @param index register index of register being removed from read bookkeeping.
 *
 * @return The number of reads(and preudo-reads) by the move after the removal.
 */
int 
MNData::removeRead(
    const TTAMachine::RegisterFile& rf,
    unsigned int index) {
    for (std::vector<TerminalRegister*>::iterator iter =
             regReads_.begin(); iter != regReads_.end(); iter++) {
        if ((*iter)->index() == static_cast<int>(index) &&
            &(*iter)->registerFile() == &rf) {
            delete *iter;
            regReads_.erase(iter);
            return regReads_.size() + pseudoRegReads_.size();
        }
    }

    for (std::vector<TerminalRegister*>::iterator iter =
             pseudoRegReads_.begin(); 
         iter != pseudoRegReads_.end(); iter++) {
        if ((*iter)->index() == static_cast<int>(index) &&
            &(*iter)->registerFile() == &rf) {
            delete *iter;
            pseudoRegReads_.erase(iter);
            return regReads_.size() + pseudoRegReads_.size();
        }
    }

    return regReads_.size() + pseudoRegReads_.size();
}

/** 
 * Removes a register write from MNData bookkeeping.
 *
 * @param reg register being removed from registes being written to.
 *
 * @return The number of writes(and preudo-writes) 
 * by the move after the removal.
 */
int 
MNData::removeWrite(
    const TTAProgram::TerminalRegister& reg) {
    return removeWrite(reg.registerFile(), reg.index());
}

/**
 * Removes a register read from MNData bookkeeping.
 * List of reads is kept for WaR bookkeeping.
 *
 * @param register register being remoed from register being read to.
 * 
 * @return The number of reads(and preudo-reads) by the move after the removal.
 */
int 
MNData::removeRead(
    const TTAProgram::TerminalRegister& reg) {
    return removeRead(reg.registerFile(), reg.index());
}

/**
 * Remove writes which are shadowed by later write
 * 
 * @param removed Writes move containing write to register(s) 
 * being removed from write list
 */
int 
MNData::removeWritesOnWAW(
    const MNData& removedWrites) {
    for (unsigned int i = 0; i < removedWrites.regWrites_.size(); i++) {
        removeWrite(*removedWrites.regWrites_[i]);
    }

    for (unsigned int i = 0; i < removedWrites.pseudoRegWrites_.size(); i++) {
        removeWrite(*removedWrites.pseudoRegWrites_[i]);
    }

    return regWrites_.size() + pseudoRegWrites_.size();
}

/**
 * Remove reads which are shadowed by later write
 * 
 * @param removedReads move containing write to register(s) 
 * being removed from read list
 */
int 
MNData::removeReadsOtherWrites(
    const MNData& removedReads) {
    for (unsigned int i = 0; i < removedReads.regWrites_.size(); i++) {
        removeRead(*removedReads.regWrites_[i]);
    }
    for (unsigned int i = 0; i < removedReads.pseudoRegWrites_.size(); i++) {
        removeRead(*removedReads.pseudoRegWrites_[i]);
    }

    return regReads_.size() + pseudoRegReads_.size();
}

/**
 * Compares if two MNData are equal, ie. point to same MoveNode and
 * Have same dependencies left.
 */
bool 
MNData::operator==(const MNData& other) const {
    return (!(*this < other) && !(other < *this));
}

/**
 * Comparator function to allow MNData to be put inside set.
 *
 * This comparison is does not make sense as general comparison 
 * for some reasonable size checking,
 * it only returns SOME order in order for set data structure to work.
 * 
 * @param other Other object for which to compare this.
 */
bool
MNData::operator<(const MNData& other) const {
    if (moveNode_ != other.moveNode_) {
        return moveNode_->nodeID() < other.moveNode_->nodeID();
    }

    if (regReads_.size() != other.regReads_.size()) {
        return regReads_.size() < other.regReads_.size();
    }

    if (pseudoRegReads_.size() != other.pseudoRegReads_.size()) {
        return pseudoRegReads_.size() < other.pseudoRegReads_.size();
    }

    if (regWrites_.size() != other.regWrites_.size()) {
        return regWrites_.size() < other.regWrites_.size();
    }

    for (unsigned int i = 0; i < regReads_.size(); i++) {
        if (!(regReads_[i]->equals(*other.regReads_[i])))
            return terminalSmaller(*regReads_[i], *other.regReads_[i]);
    }

    for (unsigned int i = 0; i < pseudoRegReads_.size(); i++) {
        if (!(pseudoRegReads_[i]->equals(*other.pseudoRegReads_[i])))
            return terminalSmaller(*pseudoRegReads_[i], 
                                   *other.pseudoRegReads_[i]);
    }

    for (unsigned int i = 0; i < regWrites_.size(); i++) {
        if (!(regWrites_[i]->equals(*other.regWrites_[i])))
            return terminalSmaller(*regWrites_[i], *other.regWrites_[i]);
    }

    for (unsigned int i = 0; i < pseudoRegWrites_.size(); i++) {
        if (!(pseudoRegWrites_[i]->equals(*other.pseudoRegWrites_[i])))
            return terminalSmaller(*pseudoRegWrites_[i], 
                                   *other.pseudoRegWrites_[i]);
    }

    return false;
}

/**
 * Compares two terminalRegisters and gives them SOME ordering.
 *
 * Made to allow MNData to be put inside std::set.
 *
 * @param t1 Left side of < operation.
 * @param t2 Right side of < operation.
 * @return true if t1 smaller than t2.
 */
bool 
MNData::terminalSmaller(
    const TTAProgram::TerminalRegister& t1,
    const TTAProgram::TerminalRegister& t2) {
    if (t1.index() != t2.index()) {
        return t1.index() < t2.index();
    }
    return &t1.registerFile() < &t2.registerFile();
}
