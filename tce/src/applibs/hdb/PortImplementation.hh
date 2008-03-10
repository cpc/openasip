/**
 * @file PortImplementation.hh
 *
 * Declaration of PortImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_PORT_IMPLEMENTATION_HH
#define TTA_PORT_IMPLEMENTATION_HH

#include <string>
#include "Exception.hh"

namespace HDB {

/**
 * Base class for RFPortImplementation and FUPortImplementation.
 */
class PortImplementation {
public:
    PortImplementation(
        const std::string& name,
        const std::string& loadPort);
    virtual ~PortImplementation();

    void setName(const std::string& name);
    std::string name() const;
    void setLoadPort(const std::string& name);
    std::string loadPort() const;

private:
    /// Name of the port.
    std::string name_;
    /// Name of the load enable port.
    std::string loadPort_;
};
}

#endif
