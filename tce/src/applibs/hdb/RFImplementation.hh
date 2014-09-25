/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file RFImplementation.hh
 *
 * Declaration of RFImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_RF_IMPLEMENTATION_HH
#define TTA_RF_IMPLEMENTATION_HH

#include <vector>
#include "HWBlockImplementation.hh"
#include "Exception.hh"
#include "HDBTypes.hh"

namespace HDB {

class RFEntry;
class RFExternalPort;
class RFPortImplementation;

/**
 * Repsesents an implementation of a RF in HDB.
 */
class RFImplementation : public HWBlockImplementation {
public:
    /// Parameter of FU implementation.
    typedef HDB::Parameter Parameter;

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
    void addExternalPort(RFExternalPort* extPort);
    void deletePort(RFPortImplementation* port)
        throw (InstanceNotFound);
    void deleteExternalPort(RFExternalPort* port)
        throw (InstanceNotFound);
    int portCount() const;
    int externalPortCount() const;
    RFPortImplementation& port(int index) const
        throw (OutOfRange);
    RFExternalPort& externalPort(int index) const
        throw (OutOfRange);

    void addParameter(
            const std::string& name,
            const std::string& type,
            const std::string& value)
            throw (IllegalParameters);
    void removeParameter(const std::string& name);
    int parameterCount() const;
    Parameter parameter(int index) const
        throw (OutOfRange);
    bool hasParameter(const std::string& name) const;

private:
    /// Vector type for RFPortImplementation.
    typedef std::vector<RFPortImplementation*> PortTable;
    /// Vector type for FUExternalPort.
    typedef std::vector<RFExternalPort*> ExternalPortTable;
    /// Vector type for parameter names.
    typedef std::vector<Parameter> ParameterTable;

    /// Name of the size parameter.
    std::string sizeParam_;
    /// Name of the width parameter.
    std::string widthParam_;
    /// Name of the guard port.
    std::string guardPort_;

    /// Contains the ports.
    PortTable ports_;
    /// Contains the external ports.
    ExternalPortTable externalPorts_;
    /// Contains the parameters.
    ParameterTable parameters_;
};
}

#endif
