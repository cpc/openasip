/**
 * @file DisassemblyFUPort.hh
 *
 * Declaration of DisassemblyFUPort.hh
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_FU_PORT_HH
#define TTA_DISASSEMBLY_FU_PORT_HH

#include "DisassemblyElement.hh"

/**
 * Represents a function unit port in the disassembler.
 */
class DisassemblyFUPort : public DisassemblyElement {
public:
    DisassemblyFUPort(std::string fuName, std::string port);
    virtual ~DisassemblyFUPort();
    virtual std::string toString() const;
private:
    /// Name of the function unit.
    std::string fuName_;
    /// Name of the function unit port.
    std::string port_;
};
#endif
