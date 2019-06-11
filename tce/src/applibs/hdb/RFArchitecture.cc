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
 * @file RFArchitecture.cc
 *
 * Implementation of RFArchitecture class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include "RFArchitecture.hh"
#include "BaseRegisterFile.hh"
#include "RegisterFile.hh"
#include "ImmediateUnit.hh"
#include "Socket.hh"
#include "Guard.hh"

using std::string;

namespace HDB {

/**
 * The constructor.
 *
 * Creates an architecture that has parameterized width and size. To set
 * fixed size or width, use setWidth or setSize method.
 *
 * @param readPorts The number of read ports.
 * @param writePorts The number of write ports.
 * @param bidirPorts The number of bidirectional ports.
 * @param maxReads The maximum number of simultaneous reads.
 * @param maxWrites The maximum number of simultaneous writes.
 * @param latency Latency of the register file.
 * @param guardSupport Tells whether the RF architecture supports guards.
 * @param guardLatency Latency between writing a register and updating the
 *                     value of guard port.
 * @exception OutOfRange If some of the arguments is out of range.
 */
RFArchitecture::RFArchitecture(
    int readPorts,
    int writePorts,
    int bidirPorts,
    int maxReads,
    int maxWrites,
    int latency,
    bool guardSupport,
    int guardLatency)
    throw (OutOfRange) :
    readPorts_(readPorts), writePorts_(writePorts), bidirPorts_(bidirPorts),
    maxReads_(maxReads), maxWrites_(maxWrites), latency_(latency), 
    guardSupport_(guardSupport), width_(0), size_(0),
    guardLatency_(guardLatency) {

    if (readPorts < 0 || writePorts < 0 || bidirPorts < 0 || maxReads < 0 ||
        maxWrites < 0 || latency < 1 || guardLatency < 0) {
        const string procName = "RFArchitecture::RFArchitecture";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }
}


/**
 * Builds RFArchitecture from RegisterFile*.
 *
 * @param rf RegisterFile*.
 */
RFArchitecture::RFArchitecture(const TTAMachine::RegisterFile* rf){

    int readPorts = 0;
    int writePorts = 0;
    int bidirPorts = 0;
    for (int i = 0; i < rf->portCount(); i++) {
        TTAMachine::Socket* input = rf->port(i)->inputSocket();
        TTAMachine::Socket* output = rf->port(i)->outputSocket();
        if (input != NULL && output != NULL) {
            bidirPorts++;
        } else if (input != NULL) {
            readPorts++;
        } else if (output != NULL) {
            writePorts++;
        }
    }

    bool guardSupport = false;
    if (rf->isRegistered()) {
        TTAMachine::Machine::BusNavigator navigator =
            rf->machine()->busNavigator();
        for (int i = 0; i < navigator.count(); i++) {
            TTAMachine::Bus* bus = navigator.item(i);
            for (int n = 0; n < bus->guardCount(); n++) {
                TTAMachine::Guard* guard = bus->guard(n);
                TTAMachine::RegisterGuard* registerGuard = 
                    dynamic_cast<TTAMachine::RegisterGuard*>(guard);
                if (registerGuard != NULL) {
                    if (registerGuard->registerFile() == rf) {
                        guardSupport = true;
                    }
                }
            }
        }
    }
    readPorts_ = readPorts;
    writePorts_ = writePorts;
    bidirPorts_ = bidirPorts;
    maxReads_ = rf->maxReads();
    maxWrites_ = rf->maxWrites();
    latency_ = 1;
    guardSupport_ = guardSupport;
    width_ = rf->width();
    size_ = rf->numberOfRegisters();
    guardLatency_ = rf->guardLatency();
}

/**
 * Builds RFArchitecture from ImmediateUnit*.
 *
 * @param imm ImmediateUnit*.
 */
RFArchitecture::RFArchitecture(const TTAMachine::ImmediateUnit* imm) {

    int readPorts = 0;
    int writePorts = 0;
    int bidirPorts = 0;
    for (int i = 0; i < imm->portCount(); i++) {
        TTAMachine::Socket* input = imm->port(i)->inputSocket();
        TTAMachine::Socket* output = imm->port(i)->outputSocket();
        if (input != NULL && output != NULL) {
            bidirPorts++;
        } else if (input != NULL) {
            readPorts++;
        } else if (output != NULL) {
            writePorts++;
        }
    }

    // immediate unit has no MaxReadWrite, no MaxReads and no guard support
    readPorts_ = readPorts;
    writePorts_ = writePorts;
    bidirPorts_ = bidirPorts;
    maxReads_ = 0;
    maxWrites_ = 0;
    latency_ = 1;
    guardSupport_ = false;
    width_ = imm->width();
    size_ = imm->numberOfRegisters();
    guardLatency_ = 0;
}

/**
 * Builds RFArchitecture from BaseRegisterFile*.
 *
 * @param baseRF BaseRegisterfile*.
 */
RFArchitecture::RFArchitecture(const TTAMachine::BaseRegisterFile* baseRF) {

    const TTAMachine::ImmediateUnit* imm = 
        dynamic_cast<const TTAMachine::ImmediateUnit*>(baseRF);
    const TTAMachine::RegisterFile* rf = 
        dynamic_cast<const TTAMachine::RegisterFile*>(baseRF);
    if (imm != NULL) {
        int readPorts = 0;
        int writePorts = 0;
        int bidirPorts = 0;
        for (int i = 0; i < imm->portCount(); i++) {
            TTAMachine::Socket* input = imm->port(i)->inputSocket();
            TTAMachine::Socket* output = imm->port(i)->outputSocket();
            if (input != NULL && output != NULL) {
                bidirPorts++;
            } else if (input != NULL) {
                readPorts++;
            } else if (output != NULL) {
                writePorts++;
            }
        }
        
        // immediate unit has no MaxReadWrite, no MaxReads and no guard support
        readPorts_ = readPorts;
        writePorts_ = writePorts;
        bidirPorts_ = bidirPorts;
        maxReads_ = 0;
        maxWrites_ = 0;
        latency_ = 1;
        guardSupport_ = false;
        width_ = imm->width();
        size_ = imm->numberOfRegisters();
        guardLatency_ = 0; 
    }
    if (rf != NULL) {
        int readPorts = 0;
        int writePorts = 0;
        int bidirPorts = 0;
        for (int i = 0; i < rf->portCount(); i++) {
            TTAMachine::Socket* input = rf->port(i)->inputSocket();
            TTAMachine::Socket* output = rf->port(i)->outputSocket();
            if (input != NULL && output != NULL) {
                bidirPorts++;
            } else if (input != NULL) {
                readPorts++;
            } else if (output != NULL) {
                writePorts++;
            }
        }
  
        bool guardSupport = false;
        if (rf->isRegistered()) {
            TTAMachine::Machine::BusNavigator navigator = 
                rf->machine()->busNavigator();
            for (int i = 0; i < navigator.count(); i++) {
                TTAMachine::Bus* bus = navigator.item(i);
                for (int n = 0; n < bus->guardCount(); n++) {
                    TTAMachine::Guard* guard = bus->guard(n);
                    TTAMachine::RegisterGuard* registerGuard = 
                        dynamic_cast<TTAMachine::RegisterGuard*>(guard);
                    if (registerGuard != NULL) {
                        if (registerGuard->registerFile() == rf) {
                            guardSupport = true;
                        }
                    }
                }
            }
        }
        readPorts_ = readPorts;
        writePorts_ = writePorts;
        bidirPorts_ = bidirPorts;
        maxReads_ = rf->maxReads();
        maxWrites_ = rf->maxWrites();
        latency_ = 1;
        guardSupport_ = guardSupport;
        width_ = rf->width();
        size_ = rf->numberOfRegisters();
        guardLatency_ = rf->guardLatency();
    }
}

/**
 * The destructor.
 */
RFArchitecture::~RFArchitecture() {
}


/**
 * Tells whether the RF has parameterized width.
 *
 * @return True if the RF has parameterized width, otherwise false.
 */
bool
RFArchitecture::hasParameterizedWidth() const {
    return width_ == 0;
}


/**
 * Tells whether the RF has parameterized size.
 *
 * @return True if the RF has parameterized size, otherwise false.
 */
bool
RFArchitecture::hasParameterizedSize() const {
    return size_ == 0;
}


/**
 * Sets the width of the register file.
 *
 * @param width The new width.
 * @exception OutOfRange If the width is less than 1.
 */
void
RFArchitecture::setWidth(int width)
    throw (OutOfRange) {

    if (width < 1) {
        const string procName = "RFArchitecture::setWidth";
        throw OutOfRange(__FILE__, __LINE__, procName);
    } else {
        width_ = width;
    }
}


/**
 * Sets the size of the register file.
 *
 * @param size The new size.
 * @exception OutOfRange If the size is less than 1.
 */
void
RFArchitecture::setSize(int size)
    throw (OutOfRange) {

    if (size < 1) {
        const string procName = "RFArchitecture::setSize";
        throw OutOfRange(__FILE__, __LINE__, procName);
    } else {
        size_ = size;
    }
}


/**
 * Returns the size of the register file.
 *
 * @return The size of the register file.
 * @exception NotAvailable If the size is parameterized.
 */
int
RFArchitecture::size() const
    throw (NotAvailable) {

    if (hasParameterizedSize()) {
        const string procName = "RFArchitecture::size";
        throw NotAvailable(__FILE__, __LINE__, procName);
    } else {
        return size_;
    }
}


/**
 * Returns the width of the register file.
 *
 * @return The width of the register file.
 * @exception NotAvailable If the width of the register file is 
 *                         parameterized.
 */
int
RFArchitecture::width() const
    throw (NotAvailable) {

    if (hasParameterizedWidth()) {
        const string procName = "RFArchitecture::width";
        throw NotAvailable(__FILE__, __LINE__, procName);
    } else {
        return width_;
    }
}


/**
 * Sets the number of read ports.
 *
 * @param portCount The number of read ports.
 * @exception OutOfRange If the given port count is negative.
 */
void
RFArchitecture::setReadPortCount(int portCount) 
    throw (OutOfRange) {

    if (portCount < 0) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }
    readPorts_ = portCount;
}


/**
 * Returns the number of read ports.
 *
 * @return The number of read ports.
 */
int
RFArchitecture::readPortCount() const {
    return readPorts_;
}


/**
 * Sets the number of write ports.
 *
 * @param portCount The number of write ports.
 * @exception OutOfRange If the given port count is smaller than 1.
 */
void
RFArchitecture::setWritePortCount(int portCount) 
    throw (OutOfRange) {

    if (portCount < 0) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }
    writePorts_ = portCount;
}


/**
 * Returns the number of write ports.
 *
 * @return The number of write ports.
 */
int
RFArchitecture::writePortCount() const {
    return writePorts_;
}


/**
 * Sets the number of bidirectional ports.
 *
 * @param portCount The number of bidirectional ports.
 * @exception OutOfRange If the given port count is negative.
 */
void
RFArchitecture::setBidirPortCount(int portCount) 
    throw (OutOfRange)  {

    if (portCount < 0) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }
    bidirPorts_ = portCount;
}


/**
 * Returns the number of bidirectional ports.
 *
 * @return The number of bidirectional ports.
 */
int
RFArchitecture::bidirPortCount() const {
    return bidirPorts_;
}


/**
 * Sets the maximum number of simultaneous reads.
 *
 * @param maxReads The new value.
 * @exception OutOfRange If the given value is negative.
 */
void
RFArchitecture::setMaxReads(int maxReads) 
    throw (OutOfRange) {

    if (maxReads < 0) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }
    maxReads_ = maxReads;
}


/**
 * Returns the maximum number of simultaneous reads.
 *
 * @return The maximum number of simultaneous reads.
 */
int
RFArchitecture::maxReads() const {
    return maxReads_;
}


/**
 * Sets the maximum number of simultaneous writes.
 *
 * @param maxWrites The new value.
 * @exception OutOfRange If the given value is negative.
 */
void
RFArchitecture::setMaxWrites(int maxWrites) 
    throw (OutOfRange) {

    if (maxWrites < 0) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }
    maxWrites_ = maxWrites;
}


/**
 * Returns the maximum number of simultaneous writes.
 *
 * @return The maximum number of simultaneous writes.
 */
int
RFArchitecture::maxWrites() const {
    return maxWrites_;
}


/**
 * Sets the latency of the register file.
 *
 * @param latency The new latency.
 * @exception OutOfRange If the given latency is smaller than 1.
 */
void
RFArchitecture::setLatency(int latency) 
    throw (OutOfRange) {

    if (latency < 1) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }
    latency_ = latency;
}


/**
 * Returns the latency of the register file.
 *
 * @return The latency of the register file.
 */
int
RFArchitecture::latency() const {
    return latency_;
}


/**
 * Sets the guard support of the register file.
 *
 * @param supported True if supported, otherwise false.
 */
void
RFArchitecture::setGuardSupport(bool supported) {
    guardSupport_ = supported;
}


/**
 * Tells whether the RF supports guards.
 *
 * @return True if the RF supports guards, otherwise false.
 */
bool
RFArchitecture::hasGuardSupport() const {
    return guardSupport_;
}


/**
 * Returns the guard latency.
 *
 * @return The guard latency.
 */
int
RFArchitecture::guardLatency() const {
    return guardLatency_;
}


/**
 * Checks whether the given RF has a mathing architecture with the given RF 
 * architecture instance.
 *
 * @param rightHand Right hand operand.
 * @return True if the architectures match, otherwise false.
 */
bool
RFArchitecture::operator==(const RFArchitecture& rightHand) const {

    if (rightHand.readPortCount() != readPortCount()) {
        return false;
    }
    if (rightHand.writePortCount() != writePortCount()) {
        return false;
    }
    if (rightHand.bidirPortCount() != bidirPortCount()) {
        return false;
    }
    if (rightHand.maxReads() != maxReads()) {
        return false;
    }
    if (rightHand.maxWrites() != maxWrites()) {
        return false;
    }
    if (rightHand.latency() != latency()) {
        return false;
    }
    if (rightHand.hasGuardSupport() != hasGuardSupport()) {
        return false;
    }
    if (rightHand.guardLatency() != guardLatency()) {
        return false;
    }
    if (rightHand.hasParameterizedSize() != hasParameterizedSize()) {
        return false;
    }
    if (!hasParameterizedSize()) {
        if (rightHand.size() != size()) {
            return false;
        }
    }
    if (rightHand.hasParameterizedWidth() != hasParameterizedWidth()) {
        return false;
    }
    if (!hasParameterizedWidth()) {
        if (rightHand.width() != width()) {
            return false;
        }
    }
    return true;
}
}
