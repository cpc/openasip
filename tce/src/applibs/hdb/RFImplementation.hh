/**
 * @file RFImplementation.hh
 *
 * Declaration of RFImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_RF_IMPLEMENTATION_HH
#define TTA_RF_IMPLEMENTATION_HH

#include <vector>
#include "HWBlockImplementation.hh"

namespace HDB {

class RFEntry;
class RFPortImplementation;

/**
 * Repsesents an implementation of a RF in HDB.
 */
class RFImplementation : public HWBlockImplementation {
public:
    RFImplementation(
        const std::string& moduleName,
        const std::string& clkPort,
        const std::string& rstPort,
        const std::string& glockPort,
        const std::string& sizeParam,
        const std::string& widthParam,
        const std::string& guardPort);
    virtual ~RFImplementation();

    RFImplementation(const RFImplementation& o);

    void setSizeParameter(const std::string& sizeParam);
    std::string sizeParameter() const;

    void setWidthParameter(const std::string& widthParam);
    std::string widthParameter() const;

    void setGuardPort(const std::string& guardPort);
    std::string guardPort() const;

    void addPort(RFPortImplementation* port);
    void deletePort(RFPortImplementation* port)
        throw (InstanceNotFound);
    int portCount() const;
    RFPortImplementation& port(int index) const
        throw (OutOfRange);

private:
    /// Vector type for RFPortImplementation.
    typedef std::vector<RFPortImplementation*> PortTable;

    /// Name of the size parameter.
    std::string sizeParam_;
    /// Name of the width parameter.
    std::string widthParam_;
    /// Name of the guard port.
    std::string guardPort_;

    /// Contains the ports.
    PortTable ports_;
};
}

#endif
