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
 * @file RegisterFile.cc
 *
 * Implementation of RegisterFile class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 17 Jun 2004 by jn, pj, jm, ll
 * @note rating: red
 */

#include "RegisterFile.hh"
#include "Machine.hh"
#include "Port.hh"
#include "Guard.hh"
#include "ControlUnit.hh"
#include "MOMTextGenerator.hh"
#include "Application.hh"
#include "ObjectState.hh"

using std::string;
using boost::format;

namespace TTAMachine {

// initialization of static data members
const string RegisterFile::OSNAME_REGISTER_FILE = "reg_file";
const string RegisterFile::OSKEY_TYPE = "type";
const string RegisterFile::OSVALUE_NORMAL = "normal";
const string RegisterFile::OSVALUE_RESERVED = "reserved";
const string RegisterFile::OSVALUE_VOLATILE = "volatile";
const string RegisterFile::OSKEY_MAX_READS = "max_r";
const string RegisterFile::OSKEY_MAX_WRITES = "max_w";
const string RegisterFile::OSKEY_GUARD_LATENCY = "g_latency";


/**
 * Constructor.
 *
 * @param name Name of the register file.
 * @param size Number of registers in the register file.
 * @param width Bit width of the registers in the register file.
 * @param maxReads Maximum simultaneous reads of a register.
 * @param maxWrites Maximum simultaneous writes of a register.
 * @param maxRw Maximum simultaneous reads of a register in the same cycle
 *              the register is written.
 * @param type Type of the register file, see RegisterFile::Type.
 * @exception OutOfRange If some of the given parameters has an illegal
 *                       value.
 * @exception InvalidName If the given name is not a valid component name.
 */
RegisterFile::RegisterFile(
    const std::string& name, unsigned int size, unsigned int width,
    unsigned int maxReads, unsigned int maxWrites, unsigned int guardLatency,
    Type type)
    : BaseRegisterFile(name, size, width),
      maxReads_(0),
      maxWrites_(0),
      guardLatency_(0),
      type_(type) {
    setMaxReads(maxReads);
    setMaxWrites(maxWrites);
    setGuardLatency(guardLatency);
}

/**
 * Constructor.
 *
 * Loads the state of the register file from the given ObjectState instance.
 * Does not load references to other components.
 *
 * @param state The ObjectState from which the name is taken.
 * @exception ObjectStateLoadingException If the given ObjectState tree is
 *                                        invalid.
 */
RegisterFile::RegisterFile(const ObjectState* state)
    : BaseRegisterFile(state),
      maxReads_(0),
      maxWrites_(0),
      guardLatency_(0),
      type_(NORMAL) {
    loadStateWithoutReferences(state);
}

/**
 * Destructor.
 */
RegisterFile::~RegisterFile() {
    unsetMachine();
}


/**
 * Returns the number of ports that can read a register all in same cycle.
 *
 * @return The number of ports that can read a register all in same cycle.
 */
int
RegisterFile::maxReads() const {
    updateMaxReadsAndWrites();
    return maxReads_;
}


/**
 * Returns the number of ports that can write a register all in same cycle.
 *
 * @return The number of ports that can write a register all in same cycle.
 */
int
RegisterFile::maxWrites() const {
    updateMaxReadsAndWrites();
    return maxWrites_;
}


/**
 * Returns the type of the register file.
 *
 * @return Type of the register file.
 */
RegisterFile::Type
RegisterFile::type() const {
    return type_;
}


/**
 * Returns true if the type of the register file is normal, otherwise false.
 *
 * @return True if the type of the register file is normal, otherwise false.
 */
bool
RegisterFile::isNormal() const {
    return (type_ == NORMAL);
}


/**
 * Returns true if the type of the register file is volatile,
 * otherwise false.
 *
 * @return True if the type of the register file is volatile,
 *         otherwise false.
 */
bool
RegisterFile::isVolatile() const {
    return (type_ == VOLATILE);
}


/**
 * Returns true if the type of the register file is reserved,
 * otherwise false.
 *
 * @return True if the type of the register file is reserved,
 *         otherwise false.
 */
bool
RegisterFile::isReserved() const {
    return (type_ == RESERVED);
}


/**
 * Sets the name of the register file.
 *
 * @param name Name of the register file.
 * @exception ComponentAlreadyExists If a register file with the given name
 *                                   is already in the same machine.
 * @exception InvalidName If the given name is not a valid component name.
 */
void
RegisterFile::setName(const std::string& name) {
    if (name == this->name()) {
        return;
    }

    if (machine() != NULL) {
        if (machine()->registerFileNavigator().hasItem(name)) {
            string procName = "RegisterFile::setName";
            throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
        } else {
            Component::setName(name);
        }
    } else {
        Component::setName(name);
    }
}

/**
 * Sets the maximum number of ports that can read a register all in the same
 * cycle. Note that this function is only needed if all of the ports are
 * unconnected, otherwise this value is computed automatically.
 *
 * The given value must be at least zero.
 *
 * @param reads Maximum number of ports.
 * @exception OutOfRange If the given number of maximum reads is out of
 *                       range.
 */
void
RegisterFile::setMaxReads(int reads) {
    if (reads < 0) {
        std::string procName = "RegisterFile::setMaxReads";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }
    maxReads_ = reads;
}

/**
 * Sets the maximum number of ports that can write a register all in the same
 * cycle. Note that this function is only needed if all of the ports are
 * unconnected, otherwise this value is computed automatically.
 *
 * The given value must be at least zero.
 *
 * @param writes Maximum number of ports.
 * @exception OutOfRange If the given number of maximum writes is out of
 *                       range.
 */
void
RegisterFile::setMaxWrites(int maxWrites) {
    if (maxWrites < 0) {
        std::string procName = "RegisterFile::setMaxWrites";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }
    maxWrites_ = maxWrites;
}

/**
 * Updates RFs max reads/writes according in/output ports.
 *
 * Port that is not assigned to a socket is considered to be a dead port, 
 * thus not counted towards the maximum number of reads or writes.
 *
 * @return True if max reads/writes changed
 * @exception OutOfRange If setMaxReads/setMaxWrites throws outOfRange
 *            exception.
 */
bool
RegisterFile::updateMaxReadsAndWrites() const {
    int reads = 0;
    int writes = 0;
    bool changed = false;

    for (int p = 0; p < portCount(); ++p) {
        if (!port(p)->isOutput() && !port(p)->isInput()) {

            // do not count unconnected ports
            //++reads;
            //++writes;

        } else {
            if (port(p)->isOutput()) {
                changed = true;
                ++reads;
            }
            if (port(p)->isInput()) {
                changed = true;
                ++writes;
            }
        }
    }

    if (changed)
    {
        maxReads_ = reads;
        maxWrites_ = writes;
    }
    return changed;
}

/**
 * Sets the type of the register file.
 *
 * @param type Type of the register file.
 */
void
RegisterFile::setType(RegisterFile::Type type) {
    type_ = type;
}


/**
 * Sets the number of registers in the register file.
 *
 * If the number of registers decreases, deletes the register guards that
 * were referencing to the registers that are removed.
 *
 * @param registers The new amount of registers.
 * @exception OutOfRange If the given number of registers is less or equal
 *                       to zero.
 */
void
RegisterFile::setNumberOfRegisters(int registers) {
    if (registers < numberOfRegisters()) {
        deleteGuards(registers);
    }
    BaseRegisterFile::setNumberOfRegisters(registers);
}

/**
 * Returns the guard latency.
 *
 * @return The guard latency.
 */
int
RegisterFile::guardLatency() const {
    return guardLatency_;
}


/**
 * Sets the guard latency.
 *
 * @param latency The new guard latency.
 * @exception OutOfRange If the given value is negative or if local + global
 *                       guard latency would be zero.
 */
void
RegisterFile::setGuardLatency(int latency) {
    if (latency < 0) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    // check that local guard latency + global guard latency > 0
    if (latency == 0 && isRegistered()) {
        ControlUnit* gcu = machine()->controlUnit();
        if (gcu != NULL) {
            int oldLatency = guardLatency();
            guardLatency_ = latency;
            try {
                // this throws OutOfRange if the local guard latency cannot
                // be zero
                gcu->setGlobalGuardLatency(gcu->globalGuardLatency());
            } catch (const OutOfRange& exception) {
                guardLatency_ = oldLatency;
                throw exception;
            }
        }
    }   

    guardLatency_ = latency;
}

/**
 * Removes the register file from machine.
 */
void
RegisterFile::unsetMachine() {
    Machine* mach = machine();
    if (mach == NULL) {
        return;
    }
    deleteGuards(0);
    BaseRegisterFile::unsetMachine();
    mach->removeRegisterFile(*this);
}


/**
 * Saves the contents to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
RegisterFile::saveState() const {

    ObjectState* regFile = BaseRegisterFile::saveState();
    regFile->setName(OSNAME_REGISTER_FILE);

    // set type
    switch(type_) {
    case NORMAL: regFile->setAttribute(OSKEY_TYPE, OSVALUE_NORMAL); break;
    case RESERVED: regFile->setAttribute(OSKEY_TYPE, OSVALUE_RESERVED);
        break;
    case VOLATILE: regFile->setAttribute(OSKEY_TYPE, OSVALUE_VOLATILE);
        break;
    default: assert(false);
    }

    // set max reads
    regFile->setAttribute(OSKEY_MAX_READS, maxReads_);

    // set max writes
    regFile->setAttribute(OSKEY_MAX_WRITES, maxWrites_);
    
    // set guard latency
    regFile->setAttribute(OSKEY_GUARD_LATENCY, guardLatency_);

    return regFile;
}


/**
 * Loads its state from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
void
RegisterFile::loadState(const ObjectState* state) {
    const string procName = "RegisterFile::loadState";
    loadStateWithoutReferences(state);
    BaseRegisterFile::loadState(state);
    MOMTextGenerator textGenerator;
}

/**
 * Loads the state of the register file without references to other
 * components.
 *
 * @param state The ObjectState instance from which the state is loaded.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
void
RegisterFile::loadStateWithoutReferences(const ObjectState* state) {
    const string procName = "RegisterFile::loadStateWithoutReferences";

    if (! (state->name() == OSNAME_REGISTER_FILE || 
           state->name() == ImmediateUnit::OSNAME_IMMEDIATE_UNIT)) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    try {
        string type = state->stringAttribute(OSKEY_TYPE);
        if (type == OSVALUE_NORMAL) {
            setType(NORMAL);
        } else if (type == OSVALUE_RESERVED) {
            setType(RESERVED);
        } else if (type == OSVALUE_VOLATILE) {
            setType(VOLATILE);
        } else {
            const string errorMsg = "Unknown register file type in "
                "ObjectState instance.";
            throw ObjectStateLoadingException(
                __FILE__, __LINE__, procName, errorMsg);
        }

        setMaxReads(state->intAttribute(OSKEY_MAX_READS));
        setMaxWrites(state->intAttribute(OSKEY_MAX_WRITES));
        setGuardLatency(state->intAttribute(OSKEY_GUARD_LATENCY));

    } catch (const Exception& e) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, e.errorMessage());
    }
}

/**
 * Deletes the guards that refer to registers would not exist if there were
 * the given number of registers in this register file.
 *
 * @param registers The number of registers.
 */
void
RegisterFile::deleteGuards(int registers) const {

    Machine* mach = machine();
    if (mach == NULL) {
        return;
    }

    // for each bus
    Machine::BusNavigator navi = mach->busNavigator();
    for (int busIndex = 0; busIndex < navi.count(); busIndex++) {
        Bus* bus = navi.item(busIndex);
        int guardIndex = 0;

        // delete register guards that refer to non-existing registers
        while (guardIndex < bus->guardCount()) {
            Guard* guard = bus->guard(guardIndex);
            RegisterGuard* regGuard =
                dynamic_cast<RegisterGuard*>(guard);
            if (regGuard != NULL && regGuard->registerFile() == this &&
                regGuard->registerIndex() >= registers) {

                // guard is removed from bus automatically
                delete regGuard;
            } else {
                guardIndex++;
            }
        }
    }
}

/**
 * Checks is this register file architecture equal with the given register file.
 *
 * Architecture equality means that register files have same values, names and
 * port names may differ.
 *
 * @param rf Register file to compare with.
 * @return True if the register files are architecture equal.
 */
bool
RegisterFile::isArchitectureEqual(const RegisterFile& rf) const {

    if (size() != rf.size()) {
        return false;
    }
    if (width() != rf.width()) {
        return false;
    }
    if (maxReads_ != rf.maxReads()) {
        return false;
    }
    if (maxWrites_ != rf.maxWrites()) {
        return false;
    }
    if (type_ != rf.type()) {
        return false;
    }
    if (guardLatency_ != rf.guardLatency()) {
        return false;
    }
    if (portCount() != rf.portCount()) {
        return false;
    }
    // ports have same width that the register file, so port widths are not
    // needed to check

    return true;
}

/**
 * Returns true if the register file is used as a guard.
 *
 * @return True if the register file is used as a guard.
 */
bool
RegisterFile::isUsedAsGuard() const {

    Machine* mach = machine();
    if (mach == NULL) {
        return false;
    }

    // for each bus
    Machine::BusNavigator navi = mach->busNavigator();
    for (int busIndex = 0; busIndex < navi.count(); busIndex++) {
        Bus* bus = navi.item(busIndex);
        int guardIndex = 0;

        // check if there are register guards that refer to this register file
        while (guardIndex < bus->guardCount()) {
            Guard* guard = bus->guard(guardIndex);
            RegisterGuard* regGuard =
                dynamic_cast<RegisterGuard*>(guard);
            if (regGuard != NULL && regGuard->registerFile() == this) {
                return true;
            } else {
                guardIndex++;
            }
        }
    }
    return false;
}

/**
 * Creates and returns a copy of the register file.
 *
 * @return Copy of register file.
 */
RegisterFile*
RegisterFile::copy() const {

    return new RegisterFile(saveState());
}

Port*
RegisterFile::firstReadPort() const {
    for (int i = 0; i < portCount(); i++) {
        Port* p = port(i);
        if (p->outputSocket() != NULL) {
            return p;
        }
    }
    return NULL;
}

Port*
RegisterFile::firstWritePort() const {
    for (int i = 0; i < portCount(); i++) {
        Port* p = port(i);
        if (p->inputSocket() != NULL) {
            return p;
        }
    }
    return NULL;
}

}
