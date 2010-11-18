/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file TransportTDGen.hh
 *
 * Declaration of TransportTDGen class.
 *
 * @author Pekka Jääskeläinen 2010 (pjaaskel)
 */

#ifndef TTA_TRANSPORT_TDGEN_HH
#define TTA_TRANSPORT_TDGEN_HH

#include <fstream>

#include "TDGen.hh"
#include "TCEString.hh"
#include "Exception.hh"
#include "tce_config.h"

namespace TTAMachine {
    class Machine;
    class BaseFUPort;
}

/**
 * TCE Backend plugin source code and .td definition generator for the
 * "transport instruction set style".
 *
 * Replicates the structure of an LLVM source code tree backend so it can
 * be easily built with the LLVM for easier debugging, but also generates
 * a backend plugin interface for faster retargeting by only needing to
 * recompile the backend parts. Uses some useful methods from the 
 * old "RISC instruction set style" TDGen.
 */
class TransportTDGen : public TDGen {
public:
    TransportTDGen(const TTAMachine::Machine& mach);
    virtual ~TransportTDGen() {}
    void generateBackend(std::string& path) 
        throw (Exception);
private:
    void writeCallingConv();
    void writeInstrFormats();
    void writeInstrInfo();
    void writeRegisterInfo();
    TCEString underScoredPortName(
        const TTAMachine::BaseFUPort& port, TCEString opName="");
    TCEString assemblyPortName(
        const TTAMachine::BaseFUPort& port, TCEString opName="");

    TCEString directoryRoot_;
    // number of GPRs to use for passing function arguments
    int funcArgRegs_;
};

#endif
