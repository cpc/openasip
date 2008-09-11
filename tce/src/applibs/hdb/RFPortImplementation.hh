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
