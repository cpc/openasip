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


class IPXactModel : public Serializable {
public:

    // TODO: append new bus interfaces here
    enum IPXactBus {
        UNKNOWN,
        HIBI
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
        int version);

    void setHdlFile(const std::string& file);

    void setFile(const std::string& file);

    void setHdlFiles(const std::vector<std::string>& files);

    void addSignal(const HDLPort& signal);

    bool addBusInterface(
        IPXactBus busType,
        const SignalMappingList& signalMap);

    static const std::string OSNAME_IPXACT_MODEL;
    static const std::string OSNAME_VENDOR;
    static const std::string OSNAME_LIBRARY;
    static const std::string OSNAME_NAME;
    static const std::string OSNAME_VERSION;
    static const std::string OSNAME_BUS_INTERFACES;
    static const std::string OSNAME_BUS_INTERFACE;
    static const std::string OSNAME_BUS_TYPE;
    static const std::string OSNAME_BUS_MASTER;
    static const std::string OSNAME_BUS_SLAVE;
    static const std::string OSNAME_BUS_SIGNAL_MAP;
    static const std::string OSNAME_BUS_SIGNAL_MAP_NAME;
    static const std::string OSNAME_BUS_SIGNAL_MAP_COMP;
    static const std::string OSNAME_BUS_SIGNAL_MAP_BUS;
    static const std::string OSNAME_MODEL;
    static const std::string OSNAME_SIGNALS;
    static const std::string OSNAME_SIGNAL;
    static const std::string OSNAME_SIGNAL_DIRECTION;
    static const std::string OSNAME_SIGNAL_LEFT;
    static const std::string OSNAME_SIGNAL_RIGHT;
    static const std::string OSNAME_FILESETS;
    static const std::string OSNAME_FILESET;
    static const std::string OSNAME_FILESET_ID;
    static const std::string OSNAME_FILE;
    static const std::string OSNAME_FILE_TYPE;

private:

    IPXactModel(const IPXactModel& old);    

    void addBusInterfaceObject(
        IPXactBus bus,
        const SignalMappingList* signalMap,
        ObjectState* parent) const;

    void addSignalObject(const HDLPort* port, ObjectState* parent) const;

    void addFileObject(
        const std::string& name,
        const std::string& type,
        ObjectState* parent) const;

    void extractVLNV(const ObjectState* root);

    void extractBusInterfaces(const ObjectState* busInterfaces);

    void extractBusInterface(const ObjectState* busInterface);

    IPXactModel::IPXactBus busType(const ObjectState* busInterface);

    void extractSignals(const ObjectState* signals);

    void extractFiles(const ObjectState* fileSets);
    
    std::string vendor_;
    std::string library_;
    std::string name_;
    int version_;
    
    std::vector<HDLPort*> signals_;

    std::vector<std::pair<IPXactBus, SignalMappingList*> > busInterfaces_;

    std::vector<std::string> hdlFiles_;
    std::vector<std::string> otherFiles_;

    static const std::string HDL_SET_ID;
    static const std::string VHDL_FILE;
    static const std::string OTHER_FILE;

    // TODO: create separate bus interface class
    static const std::string HIBI_BUS_NAME;
    static const std::string HIBI_LIBRARY;
    static const std::string HIBI_IF_NAME;
    static const std::string HIBI_VENDOR;
    static const int HIBI_VERSION;

};
#endif
