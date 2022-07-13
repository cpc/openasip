/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file IPXactModel.hh
 *
 * Declaration of IPXactModel class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#ifndef TTA_IP_XACT_MODEL_HH
#define TTA_IP_XACT_MODEL_HH

#include <string>
#include <vector>
#include "Serializable.hh"
#include "HDLPort.hh"
#include "Vlnv.hh"
#include "Netlist.hh"
#include "Parameter.hh"

class IPXactInterface;

class IPXactAddressSpace;

class IPXactModel : public Serializable {
public:

    enum BusMode {
        INVALID,
        MASTER,
        MIRRORED_MASTER,
        SLAVE,
        MIRRORED_SLAVE,
        SYSTEM,
        MIRRORED_SYSTEM,
        MONITOR
    };

    IPXactModel();

    IPXactModel(const ObjectState* state);

    virtual ~IPXactModel();

    virtual void loadState(const ObjectState* state);

    virtual ObjectState* saveState() const;

    void setVLNV(
        TCEString vendor,
        TCEString library,
        TCEString name,
        TCEString version);

    void setHdlFile(const TCEString& file);

    void setFile(const TCEString& file);

    void setHdlFiles(const std::vector<TCEString>& files);

    void addSignal(const HDLPort& signal);

    void addParameter(const ProGe::Parameter& parameter);

    void addBusInterface(IPXactInterface* interface);

    void addAddressSpace(IPXactAddressSpace* addrSpace);

    static const TCEString OSNAME_IPXACT_MODEL;
    static const TCEString OSNAME_VENDOR;
    static const TCEString OSNAME_LIBRARY;
    static const TCEString OSNAME_NAME;
    static const TCEString OSNAME_VERSION;
    static const TCEString OSNAME_BUS_INTERFACES;
    static const TCEString OSNAME_BUS_INTERFACE;
    static const TCEString OSNAME_BUS_TYPE;
    static const TCEString OSNAME_BUS_ABS_TYPE;
    static const TCEString OSNAME_BUS_MASTER;
    static const TCEString OSNAME_BUS_MIRRORED_MASTER;
    static const TCEString OSNAME_BUS_SLAVE;
    static const TCEString OSNAME_BUS_MIRRORED_SLAVE;
    static const TCEString OSNAME_BUS_SYSTEM;
    static const TCEString OSNAME_BUS_MIRRORED_SYSTEM;
    static const TCEString OSNAME_BUS_MONITOR;
    static const TCEString OSNAME_BUS_PORT_MAPS;
    static const TCEString OSNAME_BUS_PORT_MAP;
    static const TCEString OSNAME_BUS_PORT_MAP_NAME;
    static const TCEString OSNAME_BUS_PORT_MAP_COMP;
    static const TCEString OSNAME_BUS_PORT_MAP_BUS;
    static const TCEString OSNAME_MODEL;
    static const TCEString OSNAME_PORTS;
    static const TCEString OSNAME_WIRE;
    static const TCEString OSNAME_VECTOR;
    static const TCEString OSNAME_PORT;
    static const TCEString OSNAME_PORT_DIRECTION;
    static const TCEString OSNAME_PORT_LEFT;
    static const TCEString OSNAME_PORT_RIGHT;
    static const TCEString OSNAME_FILESETS;
    static const TCEString OSNAME_FILESET;
    static const TCEString OSNAME_FILE;
    static const TCEString OSNAME_FILE_NAME;
    static const TCEString OSNAME_FILE_TYPE;
    static const std::string OSNAME_ADDRESS_SPACES;
    static const std::string OSNAME_ADDRESS_SPACE;
    static const std::string OSNAME_AS_RANGE;
    static const std::string OSNAME_AS_WIDTH;
    static const std::string OSNAME_AS_MAU;
    static const TCEString OSNAME_MODEL_PARAMS;
    static const TCEString OSNAME_MODEL_PARAM;
    static const TCEString OSNAME_DISPLAY_NAME;
    static const TCEString OSNAME_VALUE;
    static const TCEString OSNAME_ATTR_DATA_TYPE;
    static const TCEString OSNAME_ATTR_FORMAT;
    static const TCEString OSNAME_ATTR_ID;
    static const TCEString OSNAME_ATTR_RESOLVE;

private:

    IPXactModel(const IPXactModel& old);    

    void addBusInterfaceObject(
        const IPXactInterface* bus,
        ObjectState* parent) const;

    void addAddressSpaceObject(
        const IPXactAddressSpace* as,
        ObjectState* parent) const;

    void addSignalObject(const HDLPort* port, ObjectState* parent) const;

    void addModelParamsObject(ObjectState* parent) const;

    void addFileObject(
        const TCEString& name,
        const TCEString& type,
        ObjectState* parent) const;

    void extractVLNV(const ObjectState* root);

    IPXact::Vlnv extractVlnvFromAttr(const ObjectState* busType) const;

    void extractBusInterfaces(const ObjectState* busInterfaces);

    void extractBusInterface(const ObjectState* busInterface);

    void extractAddressSpaces(const ObjectState* addressSpaces);

    void extractAddressSpace(const ObjectState* as);
    
    BusMode extractBusMode(const ObjectState* busInterface) const;

    void extractPortMappings(
        const ObjectState* portMaps,
        IPXactInterface& interface) const;

    void extractPortMap(
        const ObjectState* portMap,
        IPXactInterface& interface) const;


    void extractSignals(const ObjectState* signals);

    void extractModelParams(const ObjectState* modelParameters);

    void extractModelParam(const ObjectState* modelParameter);

    void extractFiles(const ObjectState* fileSets);

    IPXactInterface* interfaceByType(
        const IPXact::Vlnv& type,
        const IPXact::Vlnv& absType,
        const TCEString instanceName,
        BusMode mode) const;
    
    IPXact::Vlnv vlnv_;
    
    std::vector<HDLPort*> signals_;

    std::vector<ProGe::Parameter> parameters_;

    std::vector<IPXactInterface*> busInterfaces_;

    std::vector<IPXactAddressSpace*> addressSpaces_;

    std::vector<TCEString> hdlFiles_;
    std::vector<TCEString> otherFiles_;

    static const TCEString HDL_SET_ID;
    static const TCEString VHDL_FILE;
    static const TCEString OTHER_FILE;
    static const TCEString RESOLVE_USER;
    static const TCEString STRING_PARAM;
    static const TCEString INTEGER_PARAM;
    static const TCEString LONG_PARAM;
    static const TCEString DEV_FAMILY_GENERIC;

};
#endif
