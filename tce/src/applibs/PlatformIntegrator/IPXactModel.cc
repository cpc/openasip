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
#include "ObjectState.hh"
#include "HDLPort.hh"
#include "Conversion.hh"
using std::string;

const std::string IPXactModel::OSNAME_IPXACT_MODEL = "spirit:component";
const std::string IPXactModel::OSNAME_VENDOR = "spirit:vendor";
const std::string IPXactModel::OSNAME_LIBRARY = "spirit:library";
const std::string IPXactModel::OSNAME_NAME = "spirit:name";
const std::string IPXactModel::OSNAME_VERSION = "spirit:version";
const std::string IPXactModel::OSNAME_BUS_INTERFACES = "spirit:busInterfaces";
const std::string IPXactModel::OSNAME_BUS_INTERFACE = "spirit:busInterface";
const std::string IPXactModel::OSNAME_BUS_TYPE = "spirit:busType";
const std::string IPXactModel::OSNAME_BUS_MASTER = "spirit:master";
const std::string IPXactModel::OSNAME_BUS_SLAVE = "spirit:slave";
const std::string IPXactModel::OSNAME_BUS_SIGNAL_MAP = "spirit:signalMap";
const std::string IPXactModel::OSNAME_BUS_SIGNAL_MAP_NAME = "spirit:signalName";
const std::string IPXactModel::OSNAME_BUS_SIGNAL_MAP_COMP = 
    "spirit:componentSignalName";
const std::string IPXactModel::OSNAME_BUS_SIGNAL_MAP_BUS =
    "spirit:busSignalName";
const std::string IPXactModel::OSNAME_MODEL = "spirit:model";
const std::string IPXactModel::OSNAME_SIGNALS = "spirit:signals";
const std::string IPXactModel::OSNAME_SIGNAL = "spirit:signal";
const std::string IPXactModel::OSNAME_SIGNAL_DIRECTION = "spirit:direction";
const std::string IPXactModel::OSNAME_SIGNAL_LEFT = "spirit:left";
const std::string IPXactModel::OSNAME_SIGNAL_RIGHT = "spirit:right";
const std::string IPXactModel::OSNAME_FILESETS = "spirit:fileSets";
const std::string IPXactModel::OSNAME_FILESET = "spirit:fileSet";
const std::string IPXactModel::OSNAME_FILESET_ID = "spirit:fileSetId";
const std::string IPXactModel::OSNAME_FILE = "spirit:file";
const std::string IPXactModel::OSNAME_FILE_TYPE = "spirit:fileType";

const std::string IPXactModel::HDL_SET_ID = "hdlSources";
const std::string IPXactModel::VHDL_FILE = "vhdlSource";
const std::string IPXactModel::OTHER_FILE = "unknown";

const std::string IPXactModel::HIBI_BUS_NAME = "hibi_p";
const std::string IPXactModel::HIBI_LIBRARY = "Koski";
const std::string IPXactModel::HIBI_IF_NAME = "Hibi_if";
const std::string IPXactModel::HIBI_VENDOR = "TUT";
const int IPXactModel::HIBI_VERSION = 4;

IPXactModel::IPXactModel(): vendor_(""), library_(""), name_(""), version_(0),
                            signals_(), busInterfaces_(), hdlFiles_(),
                            otherFiles_() {
}


IPXactModel::IPXactModel(const ObjectState* state)
    throw (ObjectStateLoadingException): 
    vendor_(""), library_(""), name_(""), version_(0), signals_(),
    busInterfaces_(), hdlFiles_(), otherFiles_() {

    loadState(state);
}


IPXactModel::~IPXactModel() {

    for (unsigned int i = 0; i < signals_.size(); i++) {
        delete signals_.at(i);
    }
    for (unsigned int i = 0; i < busInterfaces_.size(); i++) {
        delete busInterfaces_.at(i).second;
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
        if (model->hasChild(OSNAME_SIGNALS)) {
            extractSignals(model->childByName(OSNAME_SIGNALS));
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
    vendor->setValue(vendor_);
    root->addChild(vendor);
    ObjectState* library = new ObjectState(OSNAME_LIBRARY);
    library->setValue(library_);
    root->addChild(library);
    ObjectState* compName = new ObjectState(OSNAME_NAME);
    compName->setValue(name_);
    root->addChild(compName);
    ObjectState* compVersion = new ObjectState(OSNAME_VERSION);
    compVersion->setValue(version_);
    root->addChild(compVersion);

    // add bus interfaces
    ObjectState* busInterfaces = new ObjectState(OSNAME_BUS_INTERFACES);
    root->addChild(busInterfaces);
    for (unsigned int i = 0; i < busInterfaces_.size(); i++) {        
        IPXactBus bus = busInterfaces_.at(i).first;
        const SignalMappingList* signalMap = busInterfaces_.at(i).second;
        ObjectState* busInterface = new ObjectState(OSNAME_BUS_INTERFACE);
        addBusInterfaceObject(i, bus, signalMap, busInterface);
        busInterfaces->addChild(busInterface);
    }
    
    // create model and add signals
    ObjectState* model = new ObjectState(OSNAME_MODEL);
    root->addChild(model);
    ObjectState* signals = new ObjectState(OSNAME_SIGNALS);
    model->addChild(signals);
    for (unsigned int i = 0; i < signals_.size(); i++) {
        ObjectState* signal = new ObjectState(OSNAME_SIGNAL);
        addSignalObject(signals_.at(i), signal);
        signals->addChild(signal);
    }

    // add hdl files
    ObjectState* fileSets = new ObjectState(OSNAME_FILESETS);
    root->addChild(fileSets);
    ObjectState* fileSet = new ObjectState(OSNAME_FILESET);
    fileSet->setAttribute(OSNAME_FILESET_ID, HDL_SET_ID);
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
    int version) {

    vendor_ = vendor;
    library_ = library;
    name_ = name;
    version_ = version;
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


bool
IPXactModel::addBusInterface(
    IPXactBus busType,
    const SignalMappingList& signalMap) {

    // TODO: change if new bus interfaces are supported
    if (busType != HIBI) {
        return false;
    }

    SignalMappingList* mappings = new SignalMappingList();
    for (unsigned int i = 0; i < signalMap.size(); i++) {
        mappings->push_back(signalMap.at(i));
    }
    
    busInterfaces_.push_back(
        std::pair<IPXactBus, SignalMappingList*>(busType, mappings));
    return true;
}

void
IPXactModel::addBusInterfaceObject(
    int id,
    IPXactBus bus,
    const SignalMappingList* signalMap,
    ObjectState* parent) const {

    ObjectState* busName = NULL;
    ObjectState* busType = NULL;
    ObjectState* role = NULL;
    if (bus == IPXactModel::HIBI) {
        busName = new ObjectState(OSNAME_NAME);
        string busInstName = HIBI_BUS_NAME + "_" + Conversion::toString(id);
        busName->setValue(busInstName);
        busType = new ObjectState(OSNAME_BUS_TYPE);
        busType->setAttribute(OSNAME_VENDOR, HIBI_VENDOR);
        busType->setAttribute(OSNAME_LIBRARY, HIBI_LIBRARY);
        busType->setAttribute(OSNAME_NAME, HIBI_IF_NAME);
        busType->setAttribute(OSNAME_VERSION, HIBI_VERSION);
        role = new ObjectState(OSNAME_BUS_MASTER);
    } else {
        string msg = "Unknown bus type";
        InvalidData* exc = 
            new InvalidData(__FILE__, __LINE__, msg, "IPXactModel");
        throw exc;
    }
    assert(busName != NULL && busType != NULL);
    parent->addChild(busName);
    parent->addChild(busType);
    if (role != NULL) {
        parent->addChild(role);
    }
    
    ObjectState* signalMapping = new ObjectState(OSNAME_BUS_SIGNAL_MAP);
    parent->addChild(signalMapping);
    for (unsigned int i = 0; i < signalMap->size(); i++) {
        ObjectState* signal = new ObjectState(OSNAME_BUS_SIGNAL_MAP_NAME);
        ObjectState* compSignal = new ObjectState(OSNAME_BUS_SIGNAL_MAP_COMP);
        ObjectState* busSignal = new ObjectState(OSNAME_BUS_SIGNAL_MAP_BUS);
        compSignal->setValue(signalMap->at(i).first);
        busSignal->setValue(signalMap->at(i).second);
        signal->addChild(compSignal);
        signal->addChild(busSignal);
        signalMapping->addChild(signal);
    }
}

void
IPXactModel::addSignalObject(
    const HDLPort* port, ObjectState* parent) const {

    ObjectState* name = new ObjectState(OSNAME_NAME);
    name->setValue(port->name());
    parent->addChild(name);

    ObjectState* direction = new ObjectState(OSNAME_SIGNAL_DIRECTION);
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
    parent->addChild(direction);
    
    int leftBorder = 0;
    int rightBorder = 0;
    if (port->type() == ProGe::BIT) {
        leftBorder = 0;
        rightBorder = 0;
    } else if (port->type() == ProGe::BIT_VECTOR) {
        rightBorder = 0;
        if (port->hasRealWidth()) {
            int width = port->realWidth();
            if (width == 0) {
                leftBorder = 0;
            } else {
                leftBorder = width-1;
            }
        } else {
            // width is unknown, guessing 1
            leftBorder = 0;
        }
    } else {
        assert(false && "unknown port type");
    }
    ObjectState* left = new ObjectState(OSNAME_SIGNAL_LEFT);
    left->setValue(leftBorder);
    parent->addChild(left);
    ObjectState* right = new ObjectState(OSNAME_SIGNAL_RIGHT);
    right->setValue(rightBorder);
    parent->addChild(right);
}

void 
IPXactModel::addFileObject(
    const std::string& name,
    const std::string& type,
    ObjectState* parent) const {

    ObjectState* fileName = new ObjectState(OSNAME_NAME);
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
    int version = 0;
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
        version = root->childByName(OSNAME_VERSION)->intValue();
    }
    setVLNV(vendor, library, name, version);
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
    IPXactBus bus = busType(busInterface);
    if (bus == UNKNOWN) {
        string msg = "Unknown bus type in file";
        ObjectStateLoadingException* exc = 
            new ObjectStateLoadingException(__FILE__, __LINE__, msg, 
                                            "IPXactModel");
        throw exc;
    }
    if (!busInterface->hasChild(OSNAME_BUS_SIGNAL_MAP)) {
        return;
    }
    SignalMappingList* busSignals = new SignalMappingList();
    const ObjectState* mapping =
        busInterface->childByName(OSNAME_BUS_SIGNAL_MAP);
    for (int i = 0; i < mapping->childCount(); i++) {
        const ObjectState* signal = mapping->child(i);
        if (signal->name() != OSNAME_BUS_SIGNAL_MAP_NAME) {
            continue;
        }
        if (signal->hasChild(OSNAME_BUS_SIGNAL_MAP_COMP) &&
            signal->hasChild(OSNAME_BUS_SIGNAL_MAP_BUS)) {
            string compSignal =  signal->
                childByName(OSNAME_BUS_SIGNAL_MAP_COMP)->stringValue();
            string busSignal = signal->
                childByName(OSNAME_BUS_SIGNAL_MAP_BUS)->stringValue();
            std::pair<string,string> signalMap(compSignal, busSignal);
            busSignals->push_back(signalMap);
        }
    }
    std::pair<IPXactBus, SignalMappingList*> busIf(bus, busSignals);
    busInterfaces_.push_back(busIf);
}


IPXactModel::IPXactBus
IPXactModel::busType(const ObjectState* busInterface) {
 
     assert(busInterface->name() == OSNAME_BUS_INTERFACE);
     IPXactBus busType = UNKNOWN;
     if (busInterface->hasChild(OSNAME_NAME)) {
         string busName = 
             busInterface->childByName(OSNAME_NAME)->stringValue();
         if (busName == HIBI_BUS_NAME) {
             busType = HIBI;
         }
     }
     return busType;
}


void
IPXactModel::extractSignals(const ObjectState* signals) {
    
    assert(signals->name() == OSNAME_SIGNALS);
    for (int i = 0; i < signals->childCount(); i++) {
        const ObjectState* signal = signals->child(i);
        if (signal->name() != OSNAME_SIGNAL) {
            continue;
        }

        string name = "";
        HDB::Direction direction = HDB::IN;
        ProGe::DataType type = ProGe::BIT;
        int width = 0;
        
        if (signal->hasChild(OSNAME_NAME)) {
            name = signal->childByName(OSNAME_NAME)->stringValue();
        }
        if (signal->hasChild(OSNAME_SIGNAL_DIRECTION)) {
            string dir =
                signal->childByName(OSNAME_SIGNAL_DIRECTION)->stringValue();
            if (dir == "in") {
                direction = HDB::IN;
            } else if (dir == "out") {
                direction = HDB::OUT;
            } else if (dir == "inout") {
                direction = HDB::BIDIR;
            } else {
                assert(false && "Unknown direction");
            }
        }
        int leftBorder = 0;
        int rightBorder = 0;
        if (signal->hasChild(OSNAME_SIGNAL_LEFT)) {
            leftBorder =
                signal->childByName(OSNAME_SIGNAL_LEFT)->intValue();
            assert(leftBorder >= 0);
        }
        if (signal->hasChild(OSNAME_SIGNAL_RIGHT)) {
            rightBorder =
                signal->childByName(OSNAME_SIGNAL_RIGHT)->intValue();
            assert(rightBorder >= 0);
        }
        if (leftBorder < rightBorder) {
            string msg = "Reversed bit order is not supported";
            UnexpectedValue* exc =
                new UnexpectedValue(__FILE__, __LINE__, msg, "IPXactModel");
            throw exc;
        }
        width = leftBorder - rightBorder + 1;
        if (width > 1) {
            type = ProGe::BIT_VECTOR;
        } else {
            type = ProGe::BIT;
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
        
        string fsId = "";
        if (!fs->hasAttribute(OSNAME_FILESET_ID)) {
            string msg = "FileSetId-attribute is missing!";
            ObjectStateLoadingException* exc = 
                new ObjectStateLoadingException(__FILE__, __LINE__, msg, 
                                                "IPXactModel");
            throw exc;
        } else {
            fsId = fs->stringAttribute(OSNAME_FILESET_ID);
        }
        if (fsId != HDL_SET_ID) {
            // unknown fileset id, ignore
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
