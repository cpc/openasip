/*
    Copyright (c) 2002-2012 Tampere University of Technology.

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
 * @file ADFCombiner.cc
 *
 * Explorer plugin that combines two input architectures (and multiplies one)
 *
 * @author Vladimir Guzma 2012 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include <vector>
#include <string>
#include "DesignSpaceExplorerPlugin.hh"
#include "DSDBManager.hh"
#include "Machine.hh"
#include "HDBRegistry.hh"
#include "StringTools.hh"
#include "RFPort.hh"
#include "FUPort.hh"
#include "ComponentImplementationSelector.hh"
#include "Exception.hh"
#include "Segment.hh"
#include "HWOperation.hh"
#include "ExecutionPipeline.hh"

//using namespace TTAProgram;
using namespace TTAMachine;
using namespace HDB;
using std::endl;

/**
 * Explorer plugin that adds machine components to a given machine.
 *
 * Supported parameters:
 *  - node, first input architecture, will be copied multiple times
 *  - node_cout, number of times the input architecture is copied to new one,
 *               default is 4
 *  - rf_size, the size of the register file connecting copied architectures,
 *              default is 4
 *  - rf_reads, number of register read ports in register file, default is 1 
 *  - rf_writes, number of register write ports in register file, default is 1
 *  - build_idf, if parameter is set the idf file is built, not set as default
 *  - extra, the other input architecture, will be copied just once
 *    If adf parameter is given the idf is built.
 */
class ADFCombiner : public DesignSpaceExplorerPlugin {
    PLUGIN_DESCRIPTION("Explorer plugin that combines two machines"
            " to produce larger machine.");

    ADFCombiner(): DesignSpaceExplorerPlugin(), 
        Node_("node.adf"),
        NodeCount_(4),
        Extra_("extra.adf") {

        // compulsory parameters
        // no compulsory parameters

        // parameters that have a default value
        addParameter(NodePN_, STRING, false, Node_);
        addParameter(NodeCountPN_, UINT, false, 
            Conversion::toString(NodeCount_));        
        addParameter(ExtraPN_, STRING, false, Extra_);
    }

    virtual bool requiresStartingPointArchitecture() const { return false; }
    virtual bool producesArchitecture() const { return true; }
    virtual bool requiresHDB() const { return false; }
    virtual bool requiresSimulationData() const { return false; }
    virtual bool requiresApplication() const { return false; }
    
    /**
     * Explorer plugin that adds machine components to a given machine with
     * adf parameter or with configuration id in dsdb.
     */
    virtual std::vector<RowID>
    explore(const RowID& /*configurationID*/, const unsigned int&) {
        readParameters();
        std::vector<RowID> result;

        // check if adf given
        if (Extra_ == "" && Node_ == "") {
            TCEString msg =
                "No node.adf or extra.adf defined. "
                "Give adfs as plugin parameters.";
            throw Exception(
                __FILE__, __LINE__, __func__, msg);
        }

        DSDBManager& dsdb = db();
        DSDBManager::MachineConfiguration conf;
        conf.hasImplementation = false;
        TTAMachine::Machine* nodeMach = NULL;
        TTAMachine::Machine* extraMach = NULL;
        TTAMachine::Machine* finalMach = NULL;                

        // load the adf from file or from dsdb
        try {
            nodeMach = TTAMachine::Machine::loadFromADF(Node_);            
        } catch (const Exception& e) {
            TCEString msg =
                "Error loading the \'" + Node_ + "\'";
            throw Exception(
                __FILE__, __LINE__, __func__, msg);
        }
        try {
            extraMach = TTAMachine::Machine::loadFromADF(Extra_);            
        } catch (const Exception& e) {
            TCEString msg =
            "Error loading the \'" + Extra_ + "\'";
            throw Exception(
                __FILE__, __LINE__, __func__, msg);
        }
        
        assert(nodeMach != NULL && extraMach != NULL);
        // Copies the extra machine to new architecture
        finalMach = new TTAMachine::Machine(*extraMach);
        // add components
        addComponents(finalMach, nodeMach, extraMach, NodeCount_);        

        // add machine to configuration
        conf.architectureID = dsdb.addArchitecture(*finalMach);

        // add new configuration to dsdb
        RowID confID = dsdb.addConfiguration(conf);
        result.push_back(confID);
        return result;
    }

private:
    /// Selector used by the plugin.
    ComponentImplementationSelector selector_;
    
    static const TCEString NodePN_;
    static const TCEString NodeCountPN_;    
    static const TCEString ExtraPN_;
    
    TCEString Node_;
    int NodeCount_;    
    TCEString Extra_;

    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        readOptionalParameter(NodePN_, Node_);
        readOptionalParameter(NodeCountPN_, NodeCount_);        
        readOptionalParameter(ExtraPN_, Extra_);
    }

    
    /**
     * Copies node.adf to the final machine nodeCount times.
     *
     * @return The initial machine of NULL if an error occurred.
     */
    void addComponents(
        TTAMachine::Machine* finalMach, 
        TTAMachine::Machine* nodeMach,                
        TTAMachine::Machine* extraMach,          
        unsigned nodeCount) {
        // Order is important here!
        // When adding busses also socket will be created
        // and the RF and FU ports will be connected to them when adding.
        addAddressSpaces(finalMach, nodeMach);                        
        addBuses(finalMach, nodeMach, nodeCount);                
        addRegisterFiles(finalMach, nodeMach, nodeCount);
        addFunctionUnits(finalMach, nodeMach, nodeCount);     
        connectRegisterFiles(finalMach, nodeMach, extraMach, nodeCount);
        connectVectorLSU(finalMach, nodeMach, extraMach, nodeCount);
    }

    /**
     * Adds buses and sockets to the machine
     *
     *
     * @return void 
     */
    
    void addBuses(
        TTAMachine::Machine* finalMach, 
        TTAMachine::Machine* nodeMach, unsigned nodeCount) {
        const Machine::SocketNavigator socketNav = nodeMach->socketNavigator();
        const TTAMachine::Machine::BusNavigator& busNav = 
        nodeMach->busNavigator();
        
        for (unsigned j = 0; j < nodeCount; j++) {
            bool socketsCreated = false;
            // Add busses
            for (int i = 0; i < busNav.count(); i++) {
                TTAMachine::Bus* addBus = busNav.item(i)->copy();
                TTAMachine::Bus* originalBus = busNav.item(i);
                
                TCEString busName = 
                    originalBus->name() + "_connect_" + Conversion::toString(j);
                addBus->setName(busName);
                try {     
                    finalMach->addBus(*addBus);
                } catch (const ComponentAlreadyExists& e) {
                    TCEString msg = "ADFCombiner: Tried to add Bus with an "
                        "already existing name (" + busName + ")";
                    throw Exception(
                        __FILE__, __LINE__, __func__, msg);
                }
                for (int k = 0; k < socketNav.count(); k++) {
                    TTAMachine::Socket* addSocket = NULL;
                    if (socketNav.item(k)->portCount() == 0) {
                        // Don't bother copying sockets that are not connected
                        // to any ports.
                        continue;
                    }
                    // When first bus added, create also new sockets
                    if (!socketsCreated) {                        
                        TCEString socketName = 
                            socketNav.item(k)->name() + "_" + 
                            Conversion::toString(j);                        
                        addSocket = new TTAMachine::Socket(socketName);   
                        try {     
                            finalMach->addSocket(*addSocket);
                        } catch (const ComponentAlreadyExists& e) {
                            TCEString msg = "ADFCombiner: Tried to add Socket with "
                            " an already existing name (" + socketName +")";
                            throw Exception(
                                __FILE__, __LINE__, __func__, msg);                            
                        }
                    } else {
                        // Sockets were create for first bus already, just pick
                        // them based on their known generated names.
                        TCEString socketName = 
                            socketNav.item(k)->name() + "_" + 
                            Conversion::toString(j);                        
                        addSocket = 
                            finalMach->socketNavigator().item(socketName);
                    }
                    // Connect sockets to bus
                    for (int l = 0; l < originalBus->segmentCount(); l++) {
                        if (socketNav.item(k)->isConnectedTo(
                            *originalBus->segment(l))) {
                            addSocket->attachBus(*addBus->segment(l));
                            addSocket->setDirection(
                                socketNav.item(k)->direction());
                        }
                    }
                }
                socketsCreated = true;                
            }
        }
    }                
        
    /**
     * Adds register file(s) to the machine
     *
     *
     * @return void 
     */

    void addRegisterFiles(
        TTAMachine::Machine* finalMach, 
        TTAMachine::Machine* nodeMach, unsigned nodeCount) {
        
        const TTAMachine::Machine::RegisterFileNavigator& RFNav = 
            nodeMach->registerFileNavigator();        
        for (unsigned j = 0; j < nodeCount; j++) {
            for (int i = 0; i < RFNav.count(); i++) {
                TTAMachine::RegisterFile* addRF = RFNav.item(i)->copy();
                TTAMachine::RegisterFile* originalRF = RFNav.item(i);
                TCEString RFName = 
                    "L_" + Conversion::toString(j) + "_" + 
                    RFNav.item(i)->name();
                addRF->setName(RFName);
                try {     
                    finalMach->addRegisterFile(*addRF);
                } catch (const ComponentAlreadyExists& e) {
                    TCEString msg = 
                        "ADFCombiner: Tried to add RF with an already"
                        "existing name (" + RFName +")";
                    throw Exception(
                        __FILE__, __LINE__, __func__, msg);
                }
                connectPorts(finalMach, originalRF, addRF, j);
            }
        }
    }
    /**
     * Adds function unit(s) to the machine
     *
     *
     * @return void 
     */
    
    void addFunctionUnits(
        TTAMachine::Machine* finalMach, 
        TTAMachine::Machine* nodeMach, unsigned nodeCount) {
        const TTAMachine::Machine::FunctionUnitNavigator& FUNav = 
            nodeMach->functionUnitNavigator();        
        for (unsigned j = 0; j < nodeCount; j++) {
            for (int i = 0; i < FUNav.count(); i++) {
                TTAMachine::FunctionUnit* addFU = FUNav.item(i)->copy();
                TTAMachine::FunctionUnit* originalFU = FUNav.item(i);                
                TCEString FUName = 
                    "L_" + Conversion::toString(j) + "_" + 
                    FUNav.item(i)->name();
                addFU->setName(FUName);                
                try {     
                    finalMach->addFunctionUnit(*addFU);
                } catch (const ComponentAlreadyExists& e) {
                    TCEString msg = 
                        "ADFCombiner: Tried to add FU with an already"
                        "existing name (" + FUName +")";
                    throw Exception(
                        __FILE__, __LINE__, __func__, msg);
                }
                if (originalFU->hasAddressSpace()) {
                    TCEString aName = originalFU->addressSpace()->name();
                    const TTAMachine::Machine::AddressSpaceNavigator& aNav = 
                        finalMach->addressSpaceNavigator();
                    assert(aNav.hasItem(aName));
                    addFU->setAddressSpace(aNav.item(aName));
                }
                connectPorts(finalMach, originalFU, addFU, j);
            }
        }
    }
    
    /**
     * Connect unit ports to sockets.
     *
     *
     * @return void 
     */
    
    void connectPorts(
        TTAMachine::Machine* finalMach,
        TTAMachine::Unit* original,
        TTAMachine::Unit* newUnit,
        int count) {
        
        for (int k = 0; k < original->portCount(); k++) {
            TTAMachine::Port* port = original->port(k);
            TTAMachine::Socket* socket = port->inputSocket();
            if (socket != NULL) {
                TCEString socketName = socket->name() + "_" +
                    Conversion::toString(count);
                TTAMachine::Socket* newSocket = 
                    finalMach->socketNavigator().item(socketName);
                newUnit->port(k)->attachSocket(*newSocket);
            }
            socket = port->outputSocket();
            if (socket != NULL) {
                TCEString socketName = socket->name() + "_" +
                    Conversion::toString(count);
                TTAMachine::Socket* newSocket = 
                    finalMach->socketNavigator().item(socketName);
                newUnit->port(k)->attachSocket(*newSocket);                        
            }
            socket = port->unconnectedSocket(0);
            if (socket != NULL) {
                TCEString socketName = socket->name() + "_" +
                    Conversion::toString(count);
                TTAMachine::Socket* newSocket = 
                    finalMach->socketNavigator().item(socketName);
                newUnit->port(k)->attachSocket(*newSocket);                        
            }
            socket = port->unconnectedSocket(1);
            if (socket != NULL) {
                TCEString socketName = socket->name() + "_" +
                    Conversion::toString(count);
                TTAMachine::Socket* newSocket = 
                    finalMach->socketNavigator().item(socketName);
                newUnit->port(k)->attachSocket(*newSocket);                        
            }                    
        }        
    }
    
    /**
     * Adds address spaces from node machine to final one, if missing.
     * Address spaces from extra machine are already in final.
     *
     *
     * @return void 
     */    
    void addAddressSpaces(
        TTAMachine::Machine* finalMach, 
        TTAMachine::Machine* nodeMach) {    
        
        const TTAMachine::Machine::AddressSpaceNavigator& ANav = 
            nodeMach->addressSpaceNavigator();        
        for (int i = 0; i < ANav.count(); i++) {
            TTAMachine::AddressSpace* origA = ANav.item(i);
            if (!finalMach->addressSpaceNavigator().hasItem(origA->name())) {            
                // Creating address space registers it.
                TTAMachine::AddressSpace* aSpace = 
                    new TTAMachine::AddressSpace(
                        origA->name(), origA->width(), origA->start(), 
                        origA->end(), *finalMach);
                assert(
                    finalMach->addressSpaceNavigator().hasItem(aSpace->name()));
            }
        }        
    }
    /**
     * Connect register files from extra and node into a ring.
     */
    void connectRegisterFiles(
        TTAMachine::Machine* finalMach, 
        TTAMachine::Machine* nodeMach, 
        TTAMachine::Machine* extraMach,
        unsigned nodeCount) {
        
        const TTAMachine::Machine::RegisterFileNavigator& nodeNav =
            nodeMach->registerFileNavigator();
        const TTAMachine::Machine::RegisterFileNavigator& finalNav =
            finalMach->registerFileNavigator();
        const TTAMachine::Machine::RegisterFileNavigator& extraNav =
            extraMach->registerFileNavigator();
            
        for (int i = 0; i < nodeNav.count(); i++) {
            if (nodeNav.item(i)->width() == 1) {
                // Don't bother connecting boolean registers atm.
                continue;
            }            
            TCEString rfName = nodeNav.item(i)->name();            
            // Connect register file between neighbouring node
            for (unsigned int j = 0; j < nodeCount -1; j++) {        
                TCEString firstName = 
                    "L_" + Conversion::toString(j) + "_" + rfName;
                TCEString secondName = 
                    "L_" + Conversion::toString(j + 1) + "_" + rfName;
                TTAMachine::RegisterFile* firstRF = finalNav.item(firstName);
                TTAMachine::RegisterFile* secondRF = finalNav.item(secondName);                
                
                TCEString busName = "connect_" + rfName + "_"
                    + Conversion::toString(j) +
                    "_" + Conversion::toString(j + 1);
                int width = std::max(firstRF->width(), secondRF->width());
                TTAMachine::Bus* newBus = createBus(finalMach, busName, width);
                
                createPortsAndSockets(finalMach, firstRF, newBus, firstName);
                createPortsAndSockets(finalMach, secondRF, newBus, secondName);                
            }
            // Add connections between RF in extra.adf with first and
            // last of the multiplied nodes
            TCEString firstName = "L_0_" + rfName;
            TCEString lastName = 
            "L_" + Conversion::toString(nodeCount -1) + "_" + rfName;            
            TTAMachine::RegisterFile* firstRF = finalNav.item(firstName);
            TTAMachine::RegisterFile* lastRF = finalNav.item(lastName);                
                
            for (int k = 0; k < extraNav.count(); k++) {
                TTAMachine::RegisterFile* extraRF =
                    finalNav.item(extraNav.item(k)->name());                
                if (extraRF->width() == 1) {
                    continue;
                }
                TCEString extraName = extraRF->name();
                int width = std::max(firstRF->width(), extraRF->width());
                TCEString busName = "connect_" + extraName + "_" + firstName;
                TTAMachine::Bus* newBus = createBus(finalMach, busName, width);
                createPortsAndSockets(finalMach, firstRF, newBus, firstName);
                createPortsAndSockets(finalMach, extraRF, newBus, extraName);                
                
                width = std::max(lastRF->width(), extraRF->width());
                busName = "connect_" + extraName + "_" + lastName;
                newBus = createBus(finalMach, busName, width);
                createPortsAndSockets(finalMach, lastRF, newBus, lastName);
                createPortsAndSockets(finalMach, extraRF, newBus, extraName);                                
            }                
        }        
    }
    /**
     * Create single bus with given name and width.
     */
    TTAMachine::Bus* createBus(
        TTAMachine::Machine* finalMach, 
        TCEString busName,
        int width) {
        TTAMachine::Bus* newBus = new TTAMachine::Bus(
            busName, width, 0,
            Machine::SIGN);
        TTAMachine::Segment* newSegment = 
            new TTAMachine::Segment(busName, *newBus);
        assert(newBus->hasSegment(newSegment->name()));
        try {     
            finalMach->addBus(*newBus);
        } catch (const ComponentAlreadyExists& e) {
            TCEString msg = 
                "ADFCombiner: Tried to add Bus with an already"
                "existing name (" + busName +")";
            throw Exception(
                __FILE__, __LINE__, __func__, msg);            
        }   
        return newBus;
    }
    /**
     * Create new ports in RF and connected them to sockets
     * and connect sockets to the bus.
     */               
    void createPortsAndSockets(
        TTAMachine::Machine* finalMach,
        TTAMachine::RegisterFile* rf,
        TTAMachine::Bus* newBus,
        TCEString name) {
        
        TTAMachine::RFPort* readPort = NULL;
        TTAMachine::RFPort* writePort = NULL;                
        
        for (int k = 0; k < rf->portCount(); k++) {
            if (rf->port(k)->name() == name + "_connect_r") {
                readPort = rf->port(k);
            }
            if (rf->port(k)->name() == name + "_connect_w") {
                writePort = rf->port(k);
            }                    
        }
        if (readPort == NULL) {
            readPort = new TTAMachine::RFPort(
                name + "_connect_r", *rf);
        }
        if (writePort == NULL) {
            writePort = new TTAMachine::RFPort(
                name + "_connect_w", *rf);        
        }
        const Machine::SocketNavigator socketNavigator = 
            finalMach->socketNavigator();
        
        TTAMachine::Socket* readSocket = NULL;
        TTAMachine::Socket* writeSocket = NULL;        
        
        if (!socketNavigator.hasItem(readPort->name())) {
            readSocket = new TTAMachine::Socket(readPort->name());                
            try {     
                finalMach->addSocket(*readSocket);
            } catch (const ComponentAlreadyExists& e) {
                TCEString msg = 
                    "ADFCombiner: Tried to add Socket with "
                    " an already existing name (" + readSocket->name() +")";
                throw Exception(
                    __FILE__, __LINE__, __func__, msg);                            
            }
        } else {
            readSocket = socketNavigator.item(readPort->name());
        }                
        if (!socketNavigator.hasItem(writePort->name())) {
            writeSocket = new TTAMachine::Socket(writePort->name());                
            try {     
                finalMach->addSocket(*writeSocket);
            } catch (const ComponentAlreadyExists& e) {
                TCEString msg = 
                "ADFCombiner: Tried to add Socket with "
                " an already existing name (" + writeSocket->name() +")";
                throw Exception(
                    __FILE__, __LINE__, __func__, msg);                            
            }
        } else {
            writeSocket = socketNavigator.item(writePort->name());
        }
        if (readPort->outputSocket() == NULL) {
            readPort->attachSocket(*readSocket);                                
        }
        if (!readSocket->isConnectedTo(*newBus->segment(0))) {
            readSocket->attachBus(*newBus->segment(0));
            readSocket->setDirection(Socket::OUTPUT);                                                    
        }
        if (writePort->inputSocket() == NULL) {               
            writePort->attachSocket(*writeSocket);                                                
        }
        if (!writeSocket->isConnectedTo(*newBus->segment(0))) {
            writeSocket->attachBus(*newBus->segment(0));                
            writeSocket->setDirection(Socket::INPUT);                                                            
        }  
    }
    /**
     * If extra has LSU that is not connected to any of the buses,
     * treat is as a vector LSU and connect address write port to extra
     * and data ports to respective number of nodes.
     */
    void connectVectorLSU(
        TTAMachine::Machine* finalMach, 
        TTAMachine::Machine* nodeMach, 
        TTAMachine::Machine* extraMach, 
        int nodeCount){
        
        const TTAMachine::Machine::FunctionUnitNavigator& FUExtraNav =
            extraMach->functionUnitNavigator();
        const TTAMachine::Machine::FunctionUnitNavigator& finalNav =
            finalMach->functionUnitNavigator();
        TTAMachine::FunctionUnit* vectorLSU = NULL;
        TTAMachine::FUPort* trigger = NULL;
        int triggerIndex = -1;
        int outputPortCount = 0;
        for (int i = 0; i < FUExtraNav.count(); i++) {
            TTAMachine::FunctionUnit* fu = FUExtraNav.item(i);
            bool unconnected = false;
            for(int j = 0; j < fu->operationPortCount(); j++) {
                if (fu->operationPort(j)->socketCount() == 0) {
                    unconnected = true;    
                    outputPortCount++;
                    if (fu->operationPort(j)->isTriggering()) {
                        triggerIndex = j;
                    }
                } else {
                    unconnected = false;
                    triggerIndex = -1;
                }
            }
            if (unconnected) {
            // We found FU in extra that is not connected to any sockets,
            // that is agreed indication that is it vector load/store and
            // needs specific connectivity.
            // Let's check if it has address space
                if (fu->hasAddressSpace()) {
                    // Ok, it seems to be load/store we look for, stop looking.
                    // Find equivalends in final architecture
                    vectorLSU = finalNav.item(fu->name());                                    
                    trigger = vectorLSU->operationPort(triggerIndex);
                    break;
                } else {
                    verboseLog("Candidate for Vector LSU does not have "
                    "address space defined - " + fu->name());
                }
            }
        }
        if (vectorLSU == NULL) {
            // No vector LSU found, nothing to do here.
            return;
        }
        
        assert(trigger != NULL);
        TTAMachine::Socket* triggerSocket = 
            new TTAMachine::Socket("vectorLSU_" + trigger->name());                
        try {     
            finalMach->addSocket(*triggerSocket);
        } catch (const ComponentAlreadyExists& e) {
            TCEString msg = 
                "ADFCombiner: Tried to add Socket with "
                " an already existing name (" + triggerSocket->name() +")";
            throw Exception(
                __FILE__, __LINE__, __func__, msg);                            
        }        
        
        trigger->attachSocket(*triggerSocket);
        
        const TTAMachine::Machine::BusNavigator& extraBusNav = 
            extraMach->busNavigator();
        const TTAMachine::Machine::BusNavigator& nodeBusNav = 
            nodeMach->busNavigator();            
        const TTAMachine::Machine::BusNavigator& finalBusNav = 
            finalMach->busNavigator();            
        // Connect trigger socket to all the buses in extra.
        for (int i = 0; i < extraBusNav.count(); i++) {
            TCEString busName = extraBusNav.item(i)->name();
            triggerSocket->attachBus(*finalBusNav.item(busName)->segment(0));                            
        }
        triggerSocket->setDirection(Socket::INPUT);    
        

        for (int j = 0; j < vectorLSU->operationCount(); j++) {
            // We run this for all the HW operations, so some 
            // sockets port connections could already exists            
            TTAMachine::HWOperation* operation = vectorLSU->operation(j);
            TTAMachine::ExecutionPipeline* pipeline = operation->pipeline();
            
            // connect write ports to nodes.            
            TTAMachine::ExecutionPipeline::OperandSet readOperandSet =
                pipeline->readOperands();
            TTAMachine::ExecutionPipeline::OperandSet::iterator it =
                readOperandSet.begin();
            for (; it != readOperandSet.end(); it++) {
                TTAMachine::Port* port = operation->port(*it);               
                if (port != trigger) {
                    // Trigger is already connected to extra, we just connect
                    // rest of writing ports.
                    TCEString socketName = "vectorLSU_" + port->name();
                    TTAMachine::Socket* inputSocket = NULL;                        
                    if (!finalMach->socketNavigator().hasItem(socketName)) {
                        inputSocket = 
                            new TTAMachine::Socket("vectorLSU_" + port->name());                
                        try {     
                            finalMach->addSocket(*inputSocket);
                        } catch (const ComponentAlreadyExists& e) {
                            TCEString msg = 
                            "ADFCombiner: Tried to add Socket with "
                            " an already existing name (" + inputSocket->name() +")";
                            throw Exception(
                                __FILE__, __LINE__, __func__, msg);                            
                        }        
                    } else {
                        inputSocket = 
                            finalMach->socketNavigator().item(socketName);
                    }
                    if (!port->isConnectedTo(*inputSocket)){
                        port->attachSocket(*inputSocket);
                    }
                    for (int i = 0; i < nodeBusNav.count(); i++) {
                        /// The name of bus is generated same way as when creating
                        /// new buses for nodes. Any change there must be reflected
                        /// here as well!
                        /// In case LSU has more ports then there are nodes
                        /// we start from beginning.
                        // Operand index 1 is trigger, usefull operand
                        // indexes starts from 2, node counting starts from 0
                        TCEString busName = 
                            nodeBusNav.item(i)->name() + "_connect_" + 
                            Conversion::toString((*it -2) % nodeCount);
                        assert(finalBusNav.hasItem(busName));
                        if (!inputSocket->isConnectedTo(
                            *finalBusNav.item(busName)->segment(0))) {
                            inputSocket->attachBus(
                                *finalBusNav.item(busName)->segment(0));                            
                        }
                    }
                    inputSocket->setDirection(Socket::INPUT);                                                                    
                }
            }
            // connect read ports to nodes.            
            TTAMachine::ExecutionPipeline::OperandSet writeOperandSet =
                pipeline->writtenOperands();
            it = writeOperandSet.begin();
            
            for (; it != writeOperandSet.end(); it++) {
                TTAMachine::Port* port = operation->port(*it);               
                assert(port != trigger);
                TCEString socketName = "vectorLSU_" + port->name();
                TTAMachine::Socket* outputSocket = NULL;                        
                if (!finalMach->socketNavigator().hasItem(socketName)) {
                    outputSocket = 
                        new TTAMachine::Socket("vectorLSU_" + port->name());                
                    try {     
                        finalMach->addSocket(*outputSocket);
                    } catch (const ComponentAlreadyExists& e) {
                        TCEString msg = 
                        "ADFCombiner: Tried to add Socket with "
                        " an already existing name (" + outputSocket->name() +")";
                        throw Exception(
                            __FILE__, __LINE__, __func__, msg);                            
                    }        
                } else {
                    outputSocket = 
                        finalMach->socketNavigator().item(socketName);
                }
                if (!port->isConnectedTo(*outputSocket)){
                    port->attachSocket(*outputSocket);
                }
                for (int i = 0; i < nodeBusNav.count(); i++) {
                    /// The name of bus is generated same way as when creating
                    /// new buses for nodes. Any change there must be reflected
                    /// here as well!
                    /// In case LSU has more ports then there are nodes
                    /// we start from beginning.
                    // Operand index 1 is trigger, usefull operand
                    // indexes starts from 2, node counting starts from 0                    
                    TCEString busName = 
                        nodeBusNav.item(i)->name() + "_connect_" + 
                        Conversion::toString((*it -2) % nodeCount);
                    assert(finalBusNav.hasItem(busName));
                    if (!outputSocket->isConnectedTo(
                            *finalBusNav.item(busName)->segment(0))) {
                        outputSocket->attachBus(
                            *finalBusNav.item(busName)->segment(0));                            
                    }
                }
                outputSocket->setDirection(Socket::OUTPUT);                                                                    
            }
        }                            
    }
};

// parameters
const TCEString ADFCombiner::NodePN_("node");
const TCEString ADFCombiner::NodeCountPN_("node_count");
const TCEString ADFCombiner::ExtraPN_("extra");


EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(ADFCombiner)
