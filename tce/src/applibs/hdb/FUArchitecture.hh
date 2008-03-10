/**
 * @file FUArchitecture.hh
 *
 * Declaration of FUArchitecture class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_ARCHITECTURE_HH
#define TTA_FU_ARCHITECTURE_HH

#include <string>
#include <set>
#include <map>
#include <vector>

#include "HWBlockArchitecture.hh"
#include "HDBTypes.hh"
#include "Exception.hh"

namespace TTAMachine {
    class FunctionUnit;
    class PipelineElement;
}

namespace HDB {

/**
 * Represents architecture of an FU in HDB.
 */
class FUArchitecture : public HWBlockArchitecture {
public:
    FUArchitecture(TTAMachine::FunctionUnit* fu);
    FUArchitecture(const FUArchitecture& o);
    virtual ~FUArchitecture();

    bool hasParameterizedWidth(const std::string& port) const;
    void setParameterizedWidth(const std::string& port);
    bool hasGuardSupport(const std::string& port) const;
    void setGuardSupport(const std::string& port);

    TTAMachine::FunctionUnit& architecture() const;
    HDB::Direction portDirection(const std::string& port) const
        throw (InstanceNotFound, InvalidData);
    
    bool operator==(const FUArchitecture& rightHand) const;
private:
    /// Struct PipelineElementUsage
    struct PipelineElementUsage {
        std::set<const TTAMachine::PipelineElement*> usage1;
        std::set<const TTAMachine::PipelineElement*> usage2;
    };

    /// typedef for PipelineElemetnUsageTable
    typedef std::vector<PipelineElementUsage> PipelineElementUsageTable;

    /// Set type for port names.
    typedef std::set<std::string> PortSet;

    /// The function unit.
    TTAMachine::FunctionUnit* fu_;
    /// Parameterized ports.
    PortSet parameterizedPorts_;
    /// Port that support guard.
    PortSet guardedPorts_;
};
}

#endif
