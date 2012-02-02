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
#include "ComponentImplementationSelector.hh"
#include "Exception.hh"
#include "Segment.hh"

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
        Node_(""),
        NodeCount_(4),
        RFSize_(4),
        RFReadPorts_(1),
        RFWritePorts_(1),
        Extra_(""),
        buildIdf_(false) {

        // compulsory parameters
        // no compulsory parameters

        // parameters that have a default value
        addParameter(NodePN_, STRING, false, Node_);
        addParameter(NodeCountPN_, UINT, false, 
            Conversion::toString(NodeCount_));        
        addParameter(RFSizePN_, UINT, false, Conversion::toString(RFSize_));
        addParameter(RFReadPortsPN_, UINT, false, 
            Conversion::toString(RFReadPorts_));
        addParameter(RFWritePortsPN_, UINT, false, 
            Conversion::toString(RFWritePorts_));
        addParameter(ExtraPN_, STRING, false, Extra_);
        addParameter(buildIdfPN_, BOOL, false, Conversion::toString(buildIdf_));
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
            std::ostringstream msg(std::ostringstream::out);
            msg << "No Node adf or Extra adf defined. "
                << "Give adfs as plugin parameters." << endl;
            verboseLog(msg.str());
            return result;
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
            extraMach = TTAMachine::Machine::loadFromADF(Extra_);
        } catch (const Exception& e) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "Error loading the adfs." << std::endl;
            verboseLog(msg.str());
            return result;
        }
        assert(nodeMach != NULL && extraMach != NULL);
        // Copies the extra machine to new architecture
        finalMach = new TTAMachine::Machine(*extraMach);
        // add components
        addComponents(finalMach, nodeMach, NodeCount_);

        if (buildIdf_) {
            try {
                // add idf to configuration
                selector_.selectComponentsToConf(conf, dsdb, finalMach);
            } catch (const Exception& e) {
                std::ostringstream msg(std::ostringstream::out);
                msg << e.errorMessage() 
                    << " " << e.fileName() 
                    << " " << e.lineNum() << std::endl;
                verboseLog(msg.str());
            }
        } else {
            conf.hasImplementation = false;
        }

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
    
    static const std::string NodePN_;
    static const std::string NodeCountPN_;    
    static const std::string RFSizePN_;
    static const std::string RFReadPortsPN_;
    static const std::string RFWritePortsPN_;
    static const std::string ExtraPN_;
    static const std::string buildIdfPN_;
    
    std::string Node_;
    int NodeCount_;    
    int RFSize_;
    int RFReadPorts_;
    int RFWritePorts_;
    /// name of the adf file if wanted to use idf generation
    std::string Extra_;
    /// do we build idf
    bool buildIdf_;

    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        readOptionalParameter(NodePN_, Node_);
        readOptionalParameter(NodeCountPN_, NodeCount_);        
        readOptionalParameter(RFSizePN_, RFSize_);
        readOptionalParameter(RFReadPortsPN_, RFReadPorts_);
        readOptionalParameter(RFWritePortsPN_, RFWritePorts_);
        readOptionalParameter(ExtraPN_, Extra_);
        readOptionalParameter(buildIdfPN_, buildIdf_);
    }

    
    /**
     * Copies node.adf to the final machine nodeCount times.
     *
     * @return The initial machine of NULL if an error occurred.
     */
    void addComponents(
        TTAMachine::Machine* finalMach, 
        TTAMachine::Machine* nodeMach,                
        unsigned nodeCount) {
        // Order is important here!
        // When adding busses also socket will be created
        // and the RF and FU ports will be connected to them when adding.
        addAddressSpaces(finalMach, nodeMach);                        
        addBuses(finalMach, nodeMach, nodeCount);                
        addRegisterFiles(finalMach, nodeMach, nodeCount);
        addFunctionUnits(finalMach, nodeMach, nodeCount);     
        connectRegisterFiles(finalMach, nodeMach, nodeCount);
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
                
                std::string busName = 
                originalBus->name() + "_" + Conversion::toString(j);
                addBus->setName(busName);
                try {     
                    finalMach->addBus(*addBus);
                } catch (const ComponentAlreadyExists& e) {
                    verboseLog("ADFCombiner: Tried to add Bus with an already"
                    "existing name (" + busName)
                    Application::exitProgram(1);
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
                        std::string socketName = 
                        socketNav.item(k)->name() + "_" + 
                        Conversion::toString(j);                        
                        addSocket = new TTAMachine::Socket(socketName);   
                        try {     
                            finalMach->addSocket(*addSocket);
                        } catch (const ComponentAlreadyExists& e) {
                            verboseLog("ADFCombiner: Tried to add Socket with "
                            " an already existing name (" + socketName)
                            Application::exitProgram(1);
                        }
                    } else {
                        // Sockets were create for first bus already, just pick
                        // them based on their known generated names.
                        std::string socketName = 
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
                std::string RFName = 
                    RFNav.item(i)->name() + "_" + Conversion::toString(j);
                addRF->setName(RFName);
                try {     
                    finalMach->addRegisterFile(*addRF);
                } catch (const ComponentAlreadyExists& e) {
                    verboseLog("ADFCombiner: Tried to add RF with an already"
                        "existing name (" + RFName)
                    Application::exitProgram(1);
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
                std::string FUName = 
                    FUNav.item(i)->name() + "_" + Conversion::toString(j);
                addFU->setName(FUName);                
                try {     
                    finalMach->addFunctionUnit(*addFU);
                } catch (const ComponentAlreadyExists& e) {
                    verboseLog("ADFCombiner: Tried to add FU with an already"
                    "existing name (" + FUName)
                    Application::exitProgram(1);
                }
                if (originalFU->hasAddressSpace()) {                    
                    
                    std::string aName = originalFU->addressSpace()->name();
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
                std::string socketName = socket->name() + "_" +
                    Conversion::toString(count);
                TTAMachine::Socket* newSocket = 
                    finalMach->socketNavigator().item(socketName);
                newUnit->port(k)->attachSocket(*newSocket);
            }
            socket = port->outputSocket();
            if (socket != NULL) {
                std::string socketName = socket->name() + "_" +
                    Conversion::toString(count);
                TTAMachine::Socket* newSocket = 
                    finalMach->socketNavigator().item(socketName);
                newUnit->port(k)->attachSocket(*newSocket);                        
            }
            socket = port->unconnectedSocket(0);
            if (socket != NULL) {
                std::string socketName = socket->name() + "_" +
                    Conversion::toString(count);
                TTAMachine::Socket* newSocket = 
                    finalMach->socketNavigator().item(socketName);
                newUnit->port(k)->attachSocket(*newSocket);                        
            }
            socket = port->unconnectedSocket(1);
            if (socket != NULL) {
                std::string socketName = socket->name() + "_" +
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
    void connectRegisterFiles(
        TTAMachine::Machine* finalMach, 
        TTAMachine::Machine* nodeMach, unsigned nodeCount) {
        
        const TTAMachine::Machine::RegisterFileNavigator& nodeNav =
            nodeMach->registerFileNavigator();
        const TTAMachine::Machine::RegisterFileNavigator& finalNav =
            finalMach->registerFileNavigator();
            
        for (int i = 0; i < nodeNav.count(); i++) {
            if (nodeNav.item(i)->width() == 1) {
                // Don't bother connecting boolean registers atm.
                continue;
            }            
            std::string rfName = nodeNav.item(i)->name();            
            
            for (unsigned int j = 0; j < nodeCount; j++) {        
                std::string firstName = 
                    rfName + "_" + Conversion::toString(j);
                std::string secondName = 
                    rfName + "_" + Conversion::toString((j + 1) % nodeCount);
                TTAMachine::RegisterFile* firstRF = finalNav.item(firstName);
                TTAMachine::RegisterFile* secondRF = finalNav.item(secondName);                
                
                std::string busName = "connect_" + rfName + "_"
                    + Conversion::toString(j) +
                    "_" + Conversion::toString((j + 1) % nodeCount);
                int width = std::max(firstRF->width(), secondRF->width());
                TTAMachine::Bus* newBus = new TTAMachine::Bus(
                    busName, width, 0,
                    Machine::SIGN);
                TTAMachine::Segment* newSegment = 
                    new TTAMachine::Segment(busName, *newBus);
                assert(newBus->hasSegment(newSegment->name()));
                try {     
                    finalMach->addBus(*newBus);
                } catch (const ComponentAlreadyExists& e) {
                    verboseLog("ADFCombiner: Tried to add Bus with an already"
                    "existing name (" + busName)
                    Application::exitProgram(1);
                }   

                TTAMachine::RFPort* firstPortRead = new TTAMachine::RFPort(
                    firstName + "_connect_r", *firstRF);
                TTAMachine::RFPort* secondPortWrite = new TTAMachine::RFPort(
                    secondName + "_connect_w", *secondRF);
                
                TTAMachine::Socket* firstSocket = 
                    new TTAMachine::Socket(firstPortRead->name());                
                try {     
                    finalMach->addSocket(*firstSocket);
                } catch (const ComponentAlreadyExists& e) {
                    verboseLog("ADFCombiner: Tried to add Socket with "
                    " an already existing name (" + firstPortRead->name())
                    Application::exitProgram(1);
                }
                    
                TTAMachine::Socket* secondSocket = 
                    new TTAMachine::Socket(secondPortWrite->name());
                try {     
                    finalMach->addSocket(*secondSocket);
                } catch (const ComponentAlreadyExists& e) {
                    verboseLog("ADFCombiner: Tried to add Socket with "
                    " an already existing name (" + secondPortWrite->name())
                    Application::exitProgram(1);
                }
                                    
                firstPortRead->attachSocket(*firstSocket);                
                firstSocket->attachBus(*newBus->segment(0));
                firstSocket->setDirection(Socket::OUTPUT);                
                
                secondPortWrite->attachSocket(*secondSocket);                                
                secondSocket->attachBus(*newBus->segment(0));                
                secondSocket->setDirection(Socket::INPUT);                                        
                
            }
        }
    }

};

// parameters
const std::string ADFCombiner::NodePN_("node");
const std::string ADFCombiner::NodeCountPN_("node_count");
const std::string ADFCombiner::RFSizePN_("rf_size");
const std::string ADFCombiner::RFReadPortsPN_("rf_reads");
const std::string ADFCombiner::RFWritePortsPN_("rf_writes");
const std::string ADFCombiner::ExtraPN_("extra");
const std::string ADFCombiner::buildIdfPN_("build_idf");

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(ADFCombiner)
