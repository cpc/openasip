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
 * @file RFArchitecture.hh
 *
 * Declaration of RFArchitecture class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_RF_ARCHITECTURE_HH
#define TTA_RF_ARCHITECTURE_HH

#include "HWBlockArchitecture.hh"
#include "Exception.hh"

namespace TTAMachine {
    class BaseRegisterFile;
    class RegisterFile;
    class ImmediateUnit;
}

namespace HDB {
    
/**
 * Represents the architecture of an RF in HDB.
 */
class RFArchitecture : public HWBlockArchitecture {
public:
    RFArchitecture(
        int readPorts,
        int writePorts,
        int bidirPorts,
        int maxReads,
        int maxWrites,
        int latency,
        bool guardSupport,
        int guardLatency = 0)
        throw (OutOfRange);
    RFArchitecture(const TTAMachine::RegisterFile* rf);
    RFArchitecture(const TTAMachine::BaseRegisterFile* rf);
    RFArchitecture(const TTAMachine::ImmediateUnit* rf);
    virtual ~RFArchitecture();

    bool hasParameterizedWidth() const;
    bool hasParameterizedSize() const;

    void setWidth(int width)
        throw (OutOfRange);
    void setSize(int size)
        throw (OutOfRange);

    int size() const
        throw (NotAvailable);
    int width() const
        throw (NotAvailable);
    
    void setReadPortCount(int portCount)
        throw (OutOfRange);
    int readPortCount() const;

    void setWritePortCount(int portCount)
        throw (OutOfRange);
    int writePortCount() const;

    void setBidirPortCount(int portCount)
        throw (OutOfRange);
    int bidirPortCount() const;

    void setMaxReads(int maxReads)
        throw (OutOfRange);
    int maxReads() const;

    void setMaxWrites(int maxWrites)
        throw (OutOfRange);
    int maxWrites() const;

    void setLatency(int latency)
        throw (OutOfRange);
    int latency() const;

    void setGuardSupport(bool supported);
    bool hasGuardSupport() const;

    int guardLatency() const;

    bool operator==(const RFArchitecture& rightHand) const;
    
private:
    /// Number of read ports.
    int readPorts_;
    /// Number of write ports.
    int writePorts_;
    /// Number of bidir ports.
    int bidirPorts_;
    /// Maximum number of simultaneous reads.
    int maxReads_;
    /// Maximum number of ports that can read a register in the same cycle
    /// in which another port writes the same register.
    int maxWrites_;
    /// The latency.
    int latency_;
    /// The guard support.
    bool guardSupport_;
    /// Width of the register file.
    int width_;
    /// Size of the register file.
    int size_;
    /// Guard latency.
    int guardLatency_;
};
}

#endif
