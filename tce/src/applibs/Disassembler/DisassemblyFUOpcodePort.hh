/**
 * @file DisassemblyFUOpcodePort.hh
 *
 * Declaration of DisassemblyFUOpcodePort class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_FU_OPCODE_PORT_HH
#define TTA_DISASSEMBLY_FU_OPCODE_PORT_HH

#include "DisassemblyElement.hh"

class DisassemblyFUOpcodePort : public DisassemblyElement {
public:
    DisassemblyFUOpcodePort(
	std::string fuName, std::string port, std::string opName);
    virtual ~DisassemblyFUOpcodePort();
    virtual std::string toString() const;
private:
    /// Name of the function unit.
    std::string fuName_;
    /// Name of the function unit port.
    std::string port_;
    /// Name of the operation.
    std::string opName_;
};

#endif
