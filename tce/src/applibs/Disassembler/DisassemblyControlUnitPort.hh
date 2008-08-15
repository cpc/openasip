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
 * @file DisassemblyControlUnitPort.hh
 *
 * Declaration of DisassemblyControlUnitPort.hh
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_CONTROL_UNIT_PORT_HH
#define TTA_DISASSEMBLY_CONTROL_UNIT_PORT_HH

#include "DisassemblyElement.hh"

/**
 * Represents a control unit port in the disassembler.
 */
class DisassemblyControlUnitPort : public DisassemblyElement {
public:
    DisassemblyControlUnitPort(std::string port);
    virtual ~DisassemblyControlUnitPort();
    virtual std::string toString() const;
private:
    /// Name of the control unit port.
    std::string port_;
};
#endif
