/**
 * @file FUExternalPort.hh
 *
 * Declaration of FUExternalPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_EXTERNAL_PORT_HH
#define TTA_FU_EXTERNAL_PORT_HH

#include <string>
#include <vector>

#include "HDBTypes.hh"
#include "Exception.hh"

namespace HDB {

class FUImplementation;

/**
 * Represents a non-architectural port of an FU implementation in HDB.
 */
class FUExternalPort {
public:
    FUExternalPort(
        const std::string& name,
        Direction direction,
        const std::string& widthFormula,
        const std::string& description,
        FUImplementation& parent);
    virtual ~FUExternalPort();

    void setName(const std::string& name);
    std::string name() const;
    void setDirection(Direction direction);
    Direction direction() const;
    void setWidthFormula(const std::string& widthFormula);
    std::string widthFormula() const;
    void setDescription(const std::string& description);
    std::string description() const;
    
    bool setParameterDependency(const std::string& parameter);
    bool unsetParameterDependency(const std::string& parameter);
    int parameterDependencyCount() const;
    std::string parameterDependency(int index) const
        throw (OutOfRange);

private:
    /// Typedef for string vector.
    typedef std::vector<std::string> ParameterTable;

    /// Name of the port.
    std::string name_;
    /// Direction of the port.
    Direction direction_;
    /// The formula for the width of the port.
    std::string widthFormula_;
    /// Description of the port.
    std::string description_;
    ParameterTable parameterDeps_;
};
}

#endif
