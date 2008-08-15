/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
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
