/**
 * @file FUPortImplementation.hh
 *
 * Declaration of FUPortImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_PORT_IMPLEMENTATION_HH
#define TTA_FU_PORT_IMPLEMENTATION_HH

#include "PortImplementation.hh"
#include "Exception.hh"

namespace HDB {

class FUImplementation;

/**
 * Represents an architectural port of an FU implementation in HDB.
 */
class FUPortImplementation : public PortImplementation {
public:
    FUPortImplementation(
        const std::string& name,
        const std::string& architecturePort,
        const std::string& widthFormula,
        const std::string& loadPort,
        const std::string& guardPort,
        FUImplementation& parent);
    virtual ~FUPortImplementation();

    void setArchitecturePort(const std::string& name);
    std::string architecturePort() const;
    void setGuardPort(const std::string& name);
    std::string guardPort() const;
    void setWidthFormula(const std::string& formula);
    std::string widthFormula() const;

private:
    /// Name of the corresponding port in architecture.
    std::string architecturePort_;
    /// The formula for the width of the port.
    std::string widthFormula_;
    /// Name of the guard port.
    std::string guardPort_;
};
}

#endif
