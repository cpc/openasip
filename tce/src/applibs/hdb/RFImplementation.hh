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
        const std::string& guardPort,
        bool sacParam = false);
    virtual ~RFImplementation();

    RFImplementation(const RFImplementation& o);

    void setSizeParameter(const std::string& sizeParam);
    std::string sizeParameter() const;

    void setWidthParameter(const std::string& widthParam);
    std::string widthParameter() const;

    void setGuardPort(const std::string& guardPort);
    std::string guardPort() const;

    void setSeparateAddressCycleParameter(bool enable);
    bool separateAddressCycleParameter() const;

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
    /// State of separate address cycle parameter.
    bool sepAddrCycleParam_;

    /// Contains the ports.
    PortTable ports_;
};
}

#endif
