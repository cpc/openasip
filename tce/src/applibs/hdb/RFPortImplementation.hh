/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file RFPortImplementation.hh
 *
 * Declaration of RFPortImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_RF_PORT_IMPLEMENTATION_HH
#define TTA_RF_PORT_IMPLEMENTATION_HH

#include "PortImplementation.hh"
#include "HDBTypes.hh"

namespace HDB {

class RFImplementation;

class RFPortImplementation : public PortImplementation {
public:
    RFPortImplementation(
        const std::string& name,
        Direction direction,
        const std::string& loadPort,
        const std::string& opcodePort,
        const std::string& opcodePortWidthFormula,
        RFImplementation& parent);
    virtual ~RFPortImplementation();

    void setDirection(Direction direction);
    Direction direction() const;
    void setOpcodePort(const std::string& name);
    std::string opcodePort() const;
    void setOpcodePortWidthFormula(const std::string& formula);
    std::string opcodePortWidthFormula() const;

private:
    /// Direction of the port.
    Direction direction_;
    /// Name of the opcode port.
    std::string opcodePort_;
    /// Width calculation formula for the opcode port.
    std::string opcodePortWidthFormula_;
};
}

#endif
