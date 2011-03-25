/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
#include "ProjectFileGenerator.hh" // for SignalMappingList
#include "HDLPort.hh"
#include "Vlnv.hh"

class IPXactInterface;


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

    IPXactModel(const ObjectState* state)
        throw (ObjectStateLoadingException);

    virtual ~IPXactModel();

    virtual void loadState(const ObjectState* state);

    virtual ObjectState* saveState() const;

    void setVLNV(
        std::string vendor,
        std::string library,
        std::string name,
        std::string version);

    void setHdlFile(const std::string& file);

    void setFile(const std::string& file);

    void setHdlFiles(const std::vector<std::string>& files);

    void addSignal(const HDLPort& signal);

    void addBusInterface(IPXactInterface* interface);

    static const std::string OSNAME_IPXACT_MODEL;
    static const std::string OSNAME_VENDOR;
    static const std::string OSNAME_LIBRARY;
    static const std::string OSNAME_NAME;
    static const std::string OSNAME_VERSION;
    static const std::string OSNAME_BUS_INTERFACES;
    static const std::string OSNAME_BUS_INTERFACE;
    static const std::string OSNAME_BUS_TYPE;
    static const std::string OSNAME_BUS_ABS_TYPE;
    static const std::string OSNAME_BUS_MASTER;
    static const std::string OSNAME_BUS_MIRRORED_MASTER;
    static const std::string OSNAME_BUS_SLAVE;
    static const std::string OSNAME_BUS_MIRRORED_SLAVE;
    static const std::string OSNAME_BUS_SYSTEM;
    static const std::string OSNAME_BUS_MIRRORED_SYSTEM;
    static const std::string OSNAME_BUS_MONITOR;
    static const std::string OSNAME_BUS_PORT_MAPS;
    static const std::string OSNAME_BUS_PORT_MAP;
    static const std::string OSNAME_BUS_PORT_MAP_NAME;
    static const std::string OSNAME_BUS_PORT_MAP_COMP;
    static const std::string OSNAME_BUS_PORT_MAP_BUS;
    static const std::string OSNAME_MODEL;
    static const std::string OSNAME_PORTS;
    static const std::string OSNAME_WIRE;
    static const std::string OSNAME_VECTOR;
    static const std::string OSNAME_PORT;
    static const std::string OSNAME_PORT_DIRECTION;
    static const std::string OSNAME_PORT_LEFT;
    static const std::string OSNAME_PORT_RIGHT;
    static const std::string OSNAME_FILESETS;
    static const std::string OSNAME_FILESET;
    static const std::string OSNAME_FILE;
    static const std::string OSNAME_FILE_NAME;
    static const std::string OSNAME_FILE_TYPE;

private:

    IPXactModel(const IPXactModel& old);    

    void addBusInterfaceObject(
        const IPXactInterface* bus,
        ObjectState* parent) const;

    void addSignalObject(const HDLPort* port, ObjectState* parent) const;

    void addFileObject(
        const std::string& name,
        const std::string& type,
        ObjectState* parent) const;

    void extractVLNV(const ObjectState* root);

    IPXact::Vlnv extractVlnvFromAttr(const ObjectState* busType) const;

    void extractBusInterfaces(const ObjectState* busInterfaces);

    void extractBusInterface(const ObjectState* busInterface);
    
    BusMode extractBusMode(const ObjectState* busInterface) const;

    void extractPortMappings(
        const ObjectState* portMaps,
        IPXactInterface& interface) const;

    void extractPortMap(
        const ObjectState* portMap,
        IPXactInterface& interface) const;


    void extractSignals(const ObjectState* signals);

    void extractFiles(const ObjectState* fileSets);

    IPXactInterface* interfaceByType(
        const IPXact::Vlnv& type,
        const IPXact::Vlnv& absType,
        const std::string instanceName,
        BusMode mode) const;
    
    IPXact::Vlnv vlnv_;
    
    std::vector<HDLPort*> signals_;

    std::vector<IPXactInterface*> busInterfaces_;

    std::vector<std::string> hdlFiles_;
    std::vector<std::string> otherFiles_;

    static const std::string HDL_SET_ID;
    static const std::string VHDL_FILE;
    static const std::string OTHER_FILE;

};
#endif
