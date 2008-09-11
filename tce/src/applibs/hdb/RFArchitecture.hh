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
