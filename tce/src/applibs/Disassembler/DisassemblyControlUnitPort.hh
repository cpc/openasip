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
