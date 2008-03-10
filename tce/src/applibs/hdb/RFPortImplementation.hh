/**
 * @file RFPortImplementation.hh
 *
 * Declaration of RFPortImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
