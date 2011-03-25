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
 * @file IPXactModel.cc
 *
 * Implementation of IPXactModel class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#include <string>
#include <vector>
#include <cassert>
#include "IPXactModel.hh"
#include "IPXactInterface.hh"
#include "IPXactClkInterface.hh"
#include "IPXactResetInterface.hh"
#include "IPXactHibiInterface.hh"
#include "ObjectState.hh"
#include "HDLPort.hh"
#include "Conversion.hh"
using std::vector;
using std::string;
using IPXact::Vlnv;

const std::string IPXactModel::OSNAME_IPXACT_MODEL = "spirit:component";
const std::string IPXactModel::OSNAME_VENDOR = "spirit:vendor";
const std::string IPXactModel::OSNAME_LIBRARY = "spirit:library";
const std::string IPXactModel::OSNAME_NAME = "spirit:name";
const std::string IPXactModel::OSNAME_VERSION = "spirit:version";
const std::string IPXactModel::OSNAME_BUS_INTERFACES = "spirit:busInterfaces";
const std::string IPXactModel::OSNAME_BUS_INTERFACE = "spirit:busInterface";
const std::string IPXactModel::OSNAME_BUS_TYPE = "spirit:busType";
const std::string IPXactModel::OSNAME_BUS_ABS_TYPE = "spirit:abstractionType";
const std::string IPXactModel::OSNAME_BUS_MASTER = "spirit:master";
const std::string IPXactModel::OSNAME_BUS_MIRRORED_MASTER =
    "spirit:mirroredMaster";
const std::string IPXactModel::OSNAME_BUS_SLAVE = "spirit:slave";
const std::string IPXactModel::OSNAME_BUS_MIRRORED_SLAVE =
    "spirit:mirroredSlave";
const std::string IPXactModel::OSNAME_BUS_SYSTEM = "spirit:system";
const std::string IPXactModel::OSNAME_BUS_MIRRORED_SYSTEM =
    "spirit:mirroredSystem";
const std::string IPXactModel::OSNAME_BUS_MONITOR = "spirit:monitor";
const std::string IPXactModel::OSNAME_BUS_PORT_MAPS = "spirit:portMaps";
const std::string IPXactModel::OSNAME_BUS_PORT_MAP = "spirit:portMap";
const std::string IPXactModel::OSNAME_BUS_PORT_MAP_NAME = "spirit:name";
const std::string IPXactModel::OSNAME_BUS_PORT_MAP_COMP = 
    "spirit:physicalPort";
const std::string IPXactModel::OSNAME_BUS_PORT_MAP_BUS =
    "spirit:logicalPort";
const std::string IPXactModel::OSNAME_MODEL = "spirit:model";
const std::string IPXactModel::OSNAME_PORTS = "spirit:ports";
const std::string IPXactModel::OSNAME_WIRE = "spirit:wire";
const std::string IPXactModel::OSNAME_VECTOR = "spirit:vector";
const std::string IPXactModel::OSNAME_PORT = "spirit:port";
const std::string IPXactModel::OSNAME_PORT_DIRECTION = "spirit:direction";
const std::string IPXactModel::OSNAME_PORT_LEFT = "spirit:left";
const std::string IPXactModel::OSNAME_PORT_RIGHT = "spirit:right";
const std::string IPXactModel::OSNAME_FILESETS = "spirit:fileSets";
const std::string IPXactModel::OSNAME_FILESET = "spirit:fileSet";
const std::string IPXactModel::OSNAME_FILE = "spirit:file";
const std::string IPXactModel::OSNAME_FILE_NAME = "spirit:name";
const std::string IPXactModel::OSNAME_FILE_TYPE = "spirit:fileType";

const std::string IPXactModel::HDL_SET_ID = "hdlSources";
const std::string IPXactModel::VHDL_FILE = "vhdlSource";
const std::string IPXactModel::OTHER_FILE = "unknown";


IPXactModel::IPXactModel(): vlnv_("","","",""),
                            signals_(), busInterfaces_(), hdlFiles_(),
                            otherFiles_() {
}


IPXactModel::IPXactModel(const ObjectState* state)
    throw (ObjectStateLoadingException): 
    vlnv_("","","",""), signals_(),
    busInterfaces_(), hdlFiles_(), otherFiles_() {

    loadState(state);
}


IPXactModel::~IPXactModel() {

    for (unsigned int i = 0; i < signals_.size(); i++) {
        delete signals_.at(i);
    }
    for (unsigned int i = 0; i < busInterfaces_.size(); i++) {
        if (busInterfaces_.at(i) != NULL) {
            delete busInterfaces_.at(i);
        }
    }
}


void
IPXactModel::loadState(const ObjectState* state) {
    
    extractVLNV(state);

    if (state->hasChild(OSNAME_BUS_INTERFACES)) {
        extractBusInterfaces(state->childByName(OSNAME_BUS_INTERFACES));
    }

    if (state->hasChild(OSNAME_MODEL)) {
        const ObjectState* model = state->childByName(OSNAME_MODEL);
        if (model->hasChild(OSNAME_PORTS)) {
            extractSignals(model->childByName(OSNAME_PORTS));
        }
    }

    if (state->hasChild(OSNAME_FILESETS)) {
        extractFiles(state->childByName(OSNAME_FILESETS));
    }
}


ObjectState*
IPXactModel::saveState() const {
    
    ObjectState* root = new ObjectState(OSNAME_IPXACT_MODEL);

    // add VLNV
    ObjectState* vendor = new ObjectState(OSNAME_VENDOR);
    vendor->setValue(vlnv_.vendor);
    root->addChild(vendor);
    ObjectState* library = new ObjectState(OSNAME_LIBRARY);
    library->setValue(vlnv_.library);
    root->addChild(library);
    ObjectState* compName = new ObjectState(OSNAME_NAME);
    compName->setValue(vlnv_.name);
    root->addChild(compName);
    ObjectState* compVersion = new ObjectState(OSNAME_VERSION);
    compVersion->setValue(vlnv_.version);
    root->addChild(compVersion);

    // add bus interfaces
    ObjectState* busInterfaces = new ObjectState(OSNAME_BUS_INTERFACES);
    root->addChild(busInterfaces);
    for (unsigned int i = 0; i < busInterfaces_.size(); i++) {        
        ObjectState* busInterface = new ObjectState(OSNAME_BUS_INTERFACE);
        addBusInterfaceObject(busInterfaces_.at(i), busInterface);
        busInterfaces->addChild(busInterface);
    }
    
    // create model and add signals
    ObjectState* model = new ObjectState(OSNAME_MODEL);
    root->addChild(model);
    ObjectState* signals = new ObjectState(OSNAME_PORTS);
    model->addChild(signals);
    for (unsigned int i = 0; i < signals_.size(); i++) {
        ObjectState* signal = new ObjectState(OSNAME_PORT);
        addSignalObject(signals_.at(i), signal);
        signals->addChild(signal);
    }

    // add hdl files
    ObjectState* fileSets = new ObjectState(OSNAME_FILESETS);
    root->addChild(fileSets);
    ObjectState* fileSet = new ObjectState(OSNAME_FILESET);
    ObjectState* fileSetName =  new ObjectState(OSNAME_FILE_NAME);
    fileSetName->setValue(HDL_SET_ID);
    fileSet->addChild(fileSetName);
    fileSets->addChild(fileSet);
    for (unsigned int i = 0; i < hdlFiles_.size(); i++) {
        ObjectState* file = new ObjectState(OSNAME_FILE);
        addFileObject(hdlFiles_.at(i), VHDL_FILE, file);
        fileSet->addChild(file);
    }
    // add other files (memory init files etc)
    for (unsigned int i = 0; i < otherFiles_.size(); i++) {
        ObjectState* file = new ObjectState(OSNAME_FILE);
        addFileObject(otherFiles_.at(i), OTHER_FILE, file);
        fileSet->addChild(file);
    }
    return root;
}


void
IPXactModel::setVLNV(
    std::string vendor,
    std::string library,
    std::string name,
    std::string version) {

    vlnv_.vendor = vendor;
    vlnv_.library = library;
    vlnv_.name = name;
    vlnv_.version = version;
}


void
IPXactModel::setHdlFile(const std::string& file) {
    
    hdlFiles_.push_back(file);
}

void
IPXactModel::setFile(const std::string& file) {

    otherFiles_.push_back(file);
}


void
IPXactModel::setHdlFiles(const std::vector<std::string>& files) {

    for (unsigned int i = 0; i < files.size(); i++) {
        setHdlFile(files.at(i));
    }
}


void
IPXactModel::addSignal(const HDLPort& signal) {

    signals_.push_back(new HDLPort(signal));
}


void
IPXactModel::addBusInterface(IPXactInterface* interface) {
    
    busInterfaces_.push_back(interface);
}


void
IPXactModel::addBusInterfaceObject(
    const IPXactInterface* bus,
    ObjectState* parent) const {

    ObjectState* busName = new ObjectState(OSNAME_NAME);
    busName->setValue(bus->instanceName());
    parent->addChild(busName);
    
    Vlnv busIfType = bus->busType();
    ObjectState* busType = new ObjectState(OSNAME_BUS_TYPE);
    busType->setAttribute(OSNAME_VENDOR, busIfType.vendor);
    busType->setAttribute(OSNAME_LIBRARY, busIfType.library);
    busType->setAttribute(OSNAME_NAME, busIfType.name);
    busType->setAttribute(OSNAME_VERSION, busIfType.version);
    parent->addChild(busType);
    
    Vlnv absType = bus->busAbstractionType();
    ObjectState* busAbsType = new ObjectState(OSNAME_BUS_ABS_TYPE);
    busAbsType->setAttribute(OSNAME_VENDOR, absType.vendor);
    busAbsType->setAttribute(OSNAME_LIBRARY, absType.library);
    busAbsType->setAttribute(OSNAME_NAME, absType.name);
    busAbsType->setAttribute(OSNAME_VERSION, absType.version);
    parent->addChild(busAbsType);
    
    ObjectState* mode = NULL;
    BusMode busMode = bus->busMode();
    if (busMode == MASTER) {
        mode = new ObjectState(OSNAME_BUS_MASTER);
    } else if (busMode == MIRRORED_MASTER) {
        mode = new ObjectState(OSNAME_BUS_MIRRORED_MASTER);
    } else if (busMode == SLAVE) {
        mode = new ObjectState(OSNAME_BUS_SLAVE);
    } else if (busMode == MIRRORED_SLAVE) {
        mode = new ObjectState(OSNAME_BUS_MIRRORED_SLAVE);
    } else {
        string msg = "Unknown bus mode!";
        InvalidData exc(__FILE__, __LINE__, "IPXactModel", msg);
        throw exc;
    }
    parent->addChild(mode);
    
    ObjectState* portMaps = new ObjectState(OSNAME_BUS_PORT_MAPS);
    parent->addChild(portMaps);

    const SignalMappingList& signalMap = bus->interfaceMapping();
    for (unsigned int i = 0; i < signalMap.size(); i++) {
        ObjectState* portMapping = new ObjectState(OSNAME_BUS_PORT_MAP);
        portMaps->addChild(portMapping);

        ObjectState* compPort = new ObjectState(OSNAME_BUS_PORT_MAP_COMP);
        ObjectState* compPortName = new ObjectState(OSNAME_BUS_PORT_MAP_NAME);
        compPortName->setValue(signalMap.at(i).first);
        compPort->addChild(compPortName);

        ObjectState* busPort = new ObjectState(OSNAME_BUS_PORT_MAP_BUS);
        ObjectState* busPortName = new ObjectState(OSNAME_BUS_PORT_MAP_NAME);
        busPortName->setValue(signalMap.at(i).second);
        busPort->addChild(busPortName);

        // in IP-XACT 1.5 bus logical port comes before physical port
        portMapping->addChild(busPort);
        portMapping->addChild(compPort);
    }
}

void
IPXactModel::addSignalObject(
    const HDLPort* port, ObjectState* parent) const {

    ObjectState* name = new ObjectState(OSNAME_NAME);
    name->setValue(port->name());
    parent->addChild(name);

    ObjectState* wire = new ObjectState(OSNAME_WIRE);
    parent->addChild(wire);

    ObjectState* direction = new ObjectState(OSNAME_PORT_DIRECTION);
    string dir = "";
    if (port->direction() == HDB::IN) {
        dir = "in";
    } else if (port->direction() == HDB::OUT) {
        dir = "out";
    } else if (port->direction() == HDB::BIDIR) {
        dir = "inout";
    } else {
        assert(false && "unknown direction");
    }
    direction->setValue(dir);
    wire->addChild(direction);
    
    ObjectState* vector = NULL;
    if (port->type() == ProGe::BIT_VECTOR) {
        int leftBorder = 0;
        int rightBorder = 0;
        vector = new ObjectState(OSNAME_VECTOR);
        if (port->hasRealWidth()) {
            int width = port->realWidth();
            if (width > 1) {
                leftBorder = width-1;
            }
        } else {
            // width is unknown, guessing 1
            leftBorder = 0;
        }
        ObjectState* left = new ObjectState(OSNAME_PORT_LEFT);
        left->setValue(leftBorder);
        vector->addChild(left);
        ObjectState* right = new ObjectState(OSNAME_PORT_RIGHT);
        right->setValue(rightBorder);
        vector->addChild(right);
    } else if (port->type() != ProGe::BIT) {
        assert(false && "unknown port type");
    }

    if (vector != NULL) {
        wire->addChild(vector);
    }
}

void 
IPXactModel::addFileObject(
    const std::string& name,
    const std::string& type,
    ObjectState* parent) const {

    ObjectState* fileName = new ObjectState(OSNAME_FILE_NAME);
    fileName->setValue(name);
    parent->addChild(fileName);
    ObjectState* fileType = new ObjectState(OSNAME_FILE_TYPE);
    fileType->setValue(type);
    parent->addChild(fileType);
}


void
IPXactModel::extractVLNV(const ObjectState* root) {

    assert(root->name() == OSNAME_IPXACT_MODEL);
    string vendor = "";
    string library = "";
    string name = "";
    string version = "";
    if (root->hasChild(OSNAME_VENDOR)) {
        vendor = root->childByName(OSNAME_VENDOR)->stringValue();
    }
    if (root->hasChild(OSNAME_LIBRARY)) {
        library = root->childByName(OSNAME_LIBRARY)->stringValue();
    }
    if (root->hasChild(OSNAME_NAME)) {
        name = root->childByName(OSNAME_NAME)->stringValue();
    }
    if (root->hasChild(OSNAME_VERSION)) {
        version = root->childByName(OSNAME_VERSION)->stringValue();
    }
    setVLNV(vendor, library, name, version);
}

IPXact::Vlnv
IPXactModel::extractVlnvFromAttr(const ObjectState* busType) const {
    
    assert(busType->name() == OSNAME_BUS_TYPE ||
           busType->name() == OSNAME_BUS_ABS_TYPE);
    IPXact::Vlnv vlnv;
    if (busType->hasAttribute(OSNAME_VENDOR)) {
        vlnv.vendor = busType->stringAttribute(OSNAME_VENDOR);
    }
    if (busType->hasAttribute(OSNAME_LIBRARY)) {
        vlnv.library = busType->stringAttribute(OSNAME_LIBRARY);
    }
    if (busType->hasAttribute(OSNAME_NAME)) {
        vlnv.name = busType->stringAttribute(OSNAME_NAME);
    }
    if (busType->hasAttribute(OSNAME_VERSION)) {
        vlnv.version = busType->stringAttribute(OSNAME_VERSION);
    }
    return vlnv;
}

void
IPXactModel::extractBusInterfaces(const ObjectState* busInterfaces) {

    assert(busInterfaces->name() == OSNAME_BUS_INTERFACES);
    if (!busInterfaces->hasChild(OSNAME_BUS_INTERFACE)) {
        return;
    }
    for (int i = 0; i < busInterfaces->childCount(); i++) {
        const ObjectState* bus = busInterfaces->child(i);
        if (bus->name() != OSNAME_BUS_INTERFACE) {
            continue;
        }
        extractBusInterface(busInterfaces->child(i));
    }
}


void
IPXactModel::extractBusInterface(const ObjectState* busInterface) {

    assert(busInterface->name() == OSNAME_BUS_INTERFACE);
    if (!busInterface->hasChild(OSNAME_NAME)) {
        string msg = "Bus has no name";
        ObjectStateLoadingException* exc = 
            new ObjectStateLoadingException(__FILE__, __LINE__,
                                            "IPXactModel", msg);
        throw exc;
    }
    if (!busInterface->hasChild(OSNAME_BUS_TYPE)) {
        string msg = "Bus has no type";
        ObjectStateLoadingException* exc = 
            new ObjectStateLoadingException(__FILE__, __LINE__,
                                            "IPXactModel", msg);
        throw exc;
    }
    if (!busInterface->hasChild(OSNAME_BUS_ABS_TYPE)) {
        string msg = "Bus has no abstraction type";
        ObjectStateLoadingException* exc = 
            new ObjectStateLoadingException(__FILE__, __LINE__, 
                                            "IPXactModel", msg);
        throw exc;
    }
    
    string instanceName =
        busInterface->childByName(OSNAME_NAME)->stringValue();
    const ObjectState* busType =
        busInterface->childByName(OSNAME_BUS_TYPE);
    const ObjectState* busAbsType = busInterface->childByName(
        OSNAME_BUS_ABS_TYPE);
    IPXact::Vlnv type = extractVlnvFromAttr(busType);
    IPXact::Vlnv absType = extractVlnvFromAttr(busAbsType);
    BusMode mode = extractBusMode(busInterface);
    
    IPXactInterface* interface =
        interfaceByType(type, absType, instanceName, mode);
    if (!interface) {
        string msg = "Unknown bus type";
        ObjectStateLoadingException* exc = 
            new ObjectStateLoadingException(__FILE__, __LINE__,
                                            "IPXactModel", msg);
        throw exc;
    }

    if (busInterface->hasChild(OSNAME_BUS_PORT_MAPS)) {
        const ObjectState* portMaps = 
            busInterface->childByName(OSNAME_BUS_PORT_MAPS);
        extractPortMappings(portMaps, *interface);
    }
    busInterfaces_.push_back(interface);
}

IPXactModel::BusMode 
IPXactModel::extractBusMode(const ObjectState* busInterface) const {

    assert(busInterface->name() == OSNAME_BUS_INTERFACE);
    BusMode mode = INVALID;
    if (busInterface->hasChild(OSNAME_BUS_MASTER)) {
        mode = MASTER;
    } else if (busInterface->hasChild(OSNAME_BUS_MIRRORED_MASTER)) {
        mode = MIRRORED_MASTER;
    } else if (busInterface->hasChild(OSNAME_BUS_SLAVE)) {
        mode = SLAVE;
    } else if (busInterface->hasChild(OSNAME_BUS_MIRRORED_SLAVE)) {
        mode = MIRRORED_SLAVE;
    } else if (busInterface->hasChild(OSNAME_BUS_SYSTEM)) {
        mode = SYSTEM;
    } else if (busInterface->hasChild(OSNAME_BUS_MIRRORED_SYSTEM)) {
        mode = MIRRORED_SYSTEM;
    } else if (busInterface->hasChild(OSNAME_BUS_MONITOR)) {
        mode = MONITOR;
    } else {
        assert(false && "Unknown bus mode!");
    }
    assert(mode != INVALID && "Bus mode is invalid");
    return mode;
}

void
IPXactModel::extractPortMappings(
    const ObjectState* portMaps,
    IPXactInterface& interface) const {

    assert(portMaps->name() == OSNAME_BUS_PORT_MAPS);
    if (!portMaps->hasChild(OSNAME_BUS_PORT_MAP)) {
        return;
    }
    for (int i = 0; i < portMaps->childCount(); i++) {
        const ObjectState* map = portMaps->child(i);
        if (map->name() == OSNAME_BUS_PORT_MAP) {
            extractPortMap(map, interface);
        }
    }
}

void
IPXactModel::extractPortMap(
    const ObjectState* portMap,
    IPXactInterface& interface) const {

    assert(portMap->name() == OSNAME_BUS_PORT_MAP);
    if (!(portMap->hasChild(OSNAME_BUS_PORT_MAP_BUS) &&
          portMap->hasChild(OSNAME_BUS_PORT_MAP_COMP))) {
        
        string msg = "Bus interface port map is invalid";
        ObjectStateLoadingException* exc = 
            new ObjectStateLoadingException(__FILE__, __LINE__,
                                            "IPXactModel", msg);
        throw exc;
    }
    
    const ObjectState* busPort =
        portMap->childByName(OSNAME_BUS_PORT_MAP_BUS);
    const ObjectState* compPort =
        portMap->childByName(OSNAME_BUS_PORT_MAP_COMP);
    if (!busPort->hasChild(OSNAME_BUS_PORT_MAP_NAME) || 
        !compPort->hasChild(OSNAME_BUS_PORT_MAP_NAME)) {
        
        string msg = "Port name missing from port map";
        ObjectStateLoadingException* exc = 
            new ObjectStateLoadingException(__FILE__, __LINE__,
                                            "IPXactModel", msg);
        throw exc;
    }
    string compPortName =
        compPort->childByName(OSNAME_BUS_PORT_MAP_NAME)->stringValue();
    string busPortName =
        busPort->childByName(OSNAME_BUS_PORT_MAP_NAME)->stringValue();

    if (busPortName.empty() || compPortName.empty()) {
        string msg = "Port name is empty in port map";
        ObjectStateLoadingException* exc = 
            new ObjectStateLoadingException(__FILE__, __LINE__,
                                            "IPXactModel", msg);
        throw exc;
    }
    interface.addSignalMapping(compPortName, busPortName);
}

void
IPXactModel::extractSignals(const ObjectState* signals) {

    assert(signals->name() == OSNAME_PORTS);

    for (int i = 0; i < signals->childCount(); i++) {
        const ObjectState* signal = signals->child(i);
        if (signal->name() != OSNAME_PORT) {
            continue;
        }

        string name = "";
        HDB::Direction direction = HDB::IN;
        ProGe::DataType type = ProGe::BIT;
        int width = 0;
        
        if (signal->hasChild(OSNAME_NAME)) {
            name = signal->childByName(OSNAME_NAME)->stringValue();
        }
        if (!signal->hasChild(OSNAME_WIRE)) {
            string msg = "Wire node not found from Port!";
            InvalidData exc(__FILE__, __LINE__, "IPXactModel", msg);
            throw exc;
        }
        const ObjectState* wire = signal->childByName(OSNAME_WIRE);
        if (wire->hasChild(OSNAME_PORT_DIRECTION)) {
            string dir =
                wire->childByName(OSNAME_PORT_DIRECTION)->stringValue();
            if (dir == "in") {
                direction = HDB::IN;
            } else if (dir == "out") {
                direction = HDB::OUT;
            } else if (dir == "inout") {
                direction = HDB::BIDIR;
            } else {
                assert(false && "Unknown direction");
            }
        } else {
            string msg = "Port does not have direction!";
            InvalidData exc(__FILE__, __LINE__, "IPXactModel", msg);
            throw exc;
        }
        
        if (wire->hasChild(OSNAME_VECTOR)) {
            type = ProGe::BIT_VECTOR;
            int leftBorder = 0;
            int rightBorder = 0;
            const ObjectState* vector = wire->childByName(OSNAME_VECTOR);
            if (vector->hasChild(OSNAME_PORT_LEFT)) {
                leftBorder =
                    vector->childByName(OSNAME_PORT_LEFT)->intValue();
                assert(leftBorder >= 0);
            }
            if (vector->hasChild(OSNAME_PORT_RIGHT)) {
                rightBorder =
                    vector->childByName(OSNAME_PORT_RIGHT)->intValue();
                assert(rightBorder >= 0);
            }
            if (leftBorder < rightBorder) {
                string msg = "Reversed bit order is not supported";
                UnexpectedValue* exc =
                    new UnexpectedValue(__FILE__, __LINE__, "IPXactModel",
                        msg);
                throw exc;
            }
            width = leftBorder - rightBorder + 1;
        } else {
            type = ProGe::BIT;
            width = 0;
        }

        string widthFormula = Conversion::toString(width);
        HDLPort* port = new HDLPort(name, widthFormula, type, direction,
                                    false, width);
        signals_.push_back(port);
    }
}

void
IPXactModel::extractFiles(const ObjectState* fileSets) {

    assert(fileSets->name() == OSNAME_FILESETS);
    for (int i = 0; i < fileSets->childCount(); i++) {
        const ObjectState* fs = fileSets->child(i);
        if (fs->name() != OSNAME_FILESET) {
            continue;
        }
        if (!fs->hasChild(OSNAME_FILE_NAME)) {
            string msg = "Fileset has no name!";
            InvalidData exc(__FILE__, __LINE__, "IPXactModel", msg);
            throw exc;
        }
        string fsName = fs->childByName(OSNAME_FILE_NAME)->stringValue();
        if (fsName != HDL_SET_ID) {
            // unknown stuff, ignore
            continue;
        }

        for (int j = 0; j < fs->childCount(); j++) {
            const ObjectState* file = fs->child(j);
            if (file->name() != OSNAME_FILE) {
                continue;
            }
            string fileName = "";
            string fileType = "";
            if (file->hasChild(OSNAME_NAME)) {
                fileName = file->childByName(OSNAME_NAME)->stringValue();
            }
            if (file->hasChild(OSNAME_FILE_TYPE)) {
                fileType = file->childByName(OSNAME_FILE_TYPE)->stringValue();
            }

            if (fileType == VHDL_FILE) {
                hdlFiles_.push_back(fileName);
            } else if (fileType == OTHER_FILE) {
                otherFiles_.push_back(fileName);
            } else {
                // unknown file type, ignore
                continue;
            }
        }
    }
}


IPXactInterface*
IPXactModel::interfaceByType(
        const IPXact::Vlnv& type,
        const IPXact::Vlnv& absType,
        const std::string instanceName,
        BusMode mode) const {
    
    IPXactInterface* interface = NULL;
    
    vector<IPXactInterface*> available;
    available.push_back(new IPXactClkInterface());
    available.push_back(new IPXactResetInterface());
    available.push_back(new IPXactHibiInterface());
    
    bool found = false;
    for (unsigned int i = 0; i < available.size(); i++) {
        if (!found) {
            if (available.at(i)->busType() == type &&
                available.at(i)->busAbstractionType() == absType &&
                available.at(i)->busMode() == mode) {
                interface = available.at(i);
                interface->setInstanceName(instanceName);
                found = true;
                continue;
            }
        }
        delete available.at(i);
    }
    return interface;
}

