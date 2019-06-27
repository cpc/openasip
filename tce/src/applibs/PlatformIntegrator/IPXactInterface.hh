/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file IPXactInterface.hh
 *
 * Declaration of IPXactInterface class.
 *
 * @author Otto Esko 2011 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#ifndef TTA_IP_XACT_INTERFACE_HH
#define TTA_IP_XACT_INTERFACE_HH

#include <string>
#include "IPXactModel.hh"
#include "NetlistBlock.hh"
#include "Vlnv.hh"
#include "PlatformIntegratorTypes.hh"

class IPXactInterface {
public:
    
    IPXactInterface();

    virtual ~IPXactInterface();

    virtual bool mapPortsToInterface(const ProGe::NetlistBlock& toplevel) = 0;

    virtual TCEString instanceName() const;

    virtual IPXact::Vlnv busType() const;

    virtual IPXact::Vlnv busAbstractionType() const;

    virtual IPXactModel::BusMode busMode() const;

    virtual const PlatInt::SignalMappingList& interfaceMapping() const;

    virtual void addSignalMapping(
        const TCEString& actualSignal,
        const TCEString& busSignal);

    void setInstanceName(const TCEString& name);

protected:

    void setBusMode(IPXactModel::BusMode mode);

    void setBusType(
        const TCEString& vendor,
        const TCEString& library,
        const TCEString& name,
        const TCEString& version);

    void setBusAbsType(
        const TCEString& vendor,
        const TCEString& library,
        const TCEString& name,
        const TCEString& version);

private:

    /// Instance Name
    TCEString instanceName_;
    /// Bus mode
    IPXactModel::BusMode busMode_;
    /// Vendor,Library,Name,Version of the bus type
    IPXact::Vlnv busType_;
    /// Vendor,Library,Name,Version of the bus abstraction type
    IPXact::Vlnv busAbstractionType_;
    /// Mapping of actual port names and interface port names
    PlatInt::SignalMappingList interfaceMap_;
};

#endif
