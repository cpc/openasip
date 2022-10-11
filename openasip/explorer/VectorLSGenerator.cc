/*
    Copyright (c) 2002-2012 Tampere University.

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
 * @file VectorLSGenerator.cc
 *
 * Explorer plugin creates wide load/store unit.
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
#include "Guard.hh"
#include "OperationPool.hh"
#include "OperationIndex.hh"
#include "Operation.hh"
#include "MathTools.hh"
#include "Conversion.hh"

//using namespace TTAProgram;
using namespace TTAMachine;
using namespace HDB;
using std::endl;

/**
 * Explorer plugin that creates wide load store unit.
 *
 * Supported parameters:
 *  - node_count, number of times the input architecture is copied to new one,
 *               default is 4.
 *  - address_spaces, semicolon separated names of the address spaces for which
 *               the LSUs will be created, default is 'data'
 */
class VectorLSGenerator : public DesignSpaceExplorerPlugin {
    PLUGIN_DESCRIPTION("Explorer plugin that creates wide load/store unit.");

    VectorLSGenerator(): DesignSpaceExplorerPlugin(), 
        nodeCount_(4),
        addressSpaces_("data") {


        // parameters that have a default value
        addParameter(NodeCountPN_, UINT, false, 
            Conversion::toString(nodeCount_));        
        addParameter(AddressSpacesPN_, STRING, false, 
            addressSpaces_);        
        
    }

    virtual bool requiresStartingPointArchitecture() const { return true; }
    virtual bool producesArchitecture() const { return true; }
    virtual bool requiresHDB() const { return false; }
    virtual bool requiresSimulationData() const { return false; }
    virtual bool requiresApplication() const { return false; }
    
    /**
     * Explorer plugin that adds machine components to a given machine with
     * adf parameter or with configuration id in dsdb.
     */
    virtual std::vector<RowID>
    explore(const RowID& configurationID, const unsigned int&) {
        readParameters();
        std::vector<RowID> result;

        DSDBManager& dsdb = db();
        DSDBManager::MachineConfiguration conf = 
            dsdb.configuration(configurationID);
        TTAMachine::Machine* mach = NULL;
        try {
            mach = dsdb.architecture(conf.architectureID);
        } catch (const Exception& e) {
            throw Exception(
                __FILE__, __LINE__, __func__, e.errorMessageStack());            
        }
        
        const TTAMachine::Machine::AddressSpaceNavigator& addrNav = 
            mach->addressSpaceNavigator();
        TTAMachine::AddressSpace* addrSpace = NULL;
        std::vector<TCEString> addressSpaces = 
            StringTools::chopString(addressSpaces_, ";");
        for (unsigned int i = 0; i < addressSpaces.size(); i++) {
            if (!addrNav.hasItem(addressSpaces[i])) {
                TCEString msg = "Original architecture does not have \"" 
                    + addressSpaces[i] +"\" address space. Not adding Vector LSU.";
                throw Exception(
                    __FILE__, __LINE__, __func__, msg);                        
            }

            addrSpace = addrNav.item(addressSpaces[i]);
            
            if (addrSpace == NULL) {
            }
            TTAMachine::FunctionUnit* lsUnit = 
                new TTAMachine::FunctionUnit("VectorLSU_" + addrSpace->name());

            createVectorLSU(*lsUnit, *addrSpace);
            
            try {
                mach->addFunctionUnit(*lsUnit);
            } catch (const Exception& e) {
                throw Exception(
                    __FILE__, __LINE__, __func__, e.errorMessageStack());                                    
            }
            try {
                lsUnit->setAddressSpace(addrSpace);
            } catch (const Exception& e) {
                throw Exception(
                    __FILE__, __LINE__, __func__, e.errorMessageStack());                                                
            }
        }
        
        DSDBManager::MachineConfiguration tempConf;
        try {
            tempConf.architectureID = dsdb.addArchitecture(*mach);
            tempConf.hasImplementation = false;
        } catch (const RelationalDBException& e) {
            TCEString msg = "Error while adding ADF to the dsdb. "
                "ADF probably too big.";
            throw Exception(
                __FILE__, __LINE__, __func__, msg);                                                
        }        
        // add config to database to pass it to
        // RemoveUnconnectedComponents plugin
        RowID tempConfID = dsdb.addConfiguration(tempConf);
        result.push_back(tempConfID);
        return result;
        
    }

private:
    /// Selector used by the plugin.
    ComponentImplementationSelector selector_;
    
    static const TCEString NodeCountPN_;    
    static const TCEString AddressSpacesPN_;       
    
    int nodeCount_;    
    TCEString addressSpaces_;

    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        readOptionalParameter(NodeCountPN_, nodeCount_);
        readOptionalParameter(AddressSpacesPN_, addressSpaces_);                
    }
    /**
     * Adds operation to function unit, sets the port bindings and pipeline
     * parameters.
     */
    void addOperation(
        TTAMachine::FunctionUnit& lsUnit, Operation& op, bool scalar) {
        
        TTAMachine::HWOperation* hwOp = 
            new TTAMachine::HWOperation(op.name(), lsUnit); 
        TTAMachine::ExecutionPipeline* pipeline = hwOp->pipeline();
        
        // Specificly define the triggering port
        hwOp->bindPort(1, *lsUnit.operationPort("in1t"));          
        pipeline->addPortRead(1,0,1);
        if (scalar) {
            if (op.numberOfInputs() == 2) {
                // this is stw operation
                hwOp->bindPort(2, *lsUnit.operationPort("in_extra2"));
                pipeline->addPortRead(2, 0, 1);
            }
            if (op.numberOfOutputs() == 1) {
                // this is ldw operation
                hwOp->bindPort(2, *lsUnit.operationPort("out_extra1"));
                pipeline->addPortWrite(2, 2, 1);
            }
            assert(hwOp->isBound(*lsUnit.operationPort("in1t")));            
            return;
        }
        // Add other inputs, after triggering port.
        for (int i = 1; i < op.numberOfInputs(); i++) {
            hwOp->bindPort(
                i+1, *lsUnit.operationPort("in" + Conversion::toString(i+1)));
            pipeline->addPortRead(i+1,0,1);            
        }
        // Add other outputs, after triggering port.        
        for (int i = 1; i <= op.numberOfOutputs(); i++) {
            hwOp->bindPort(
                i+1, *lsUnit.operationPort("out" + Conversion::toString(i)));
            pipeline->addPortWrite(i+1,2,1);                        
        }
        assert(hwOp->isBound(*lsUnit.operationPort("in1t")));
    }
    
    /**
     * Create single instance of vector load store unit, starting from empty
     * unit.
     */
    void createVectorLSU(
        TTAMachine::FunctionUnit& lsUnit, 
        TTAMachine::AddressSpace& addrSpace) {
        
        TTAMachine::FUPort* trigger = 
            new TTAMachine::FUPort("in1t", 
                MathTools::requiredBits(addrSpace.end()), 
                lsUnit, true, true);
        assert(trigger->isTriggering());
        int width = (nodeCount_ >= 8) ? 8 : 
            (nodeCount_ >= 4) ? 4 : 
            (nodeCount_ >= 2) ? 2 : 0;
        if (width == 0) {
            TCEString msg = "No reason for creating wide LSU since number" 
                " of nodes in cluster is just " + Conversion::toString(nodeCount_);
            throw Exception(
                __FILE__, __LINE__, __func__, msg);                                    
        }
        for (int i = 1; i <= width; i++) {
            TTAMachine::FUPort* inPort =
                new TTAMachine::FUPort(
                    "in" + Conversion::toString(i+1), 32, lsUnit, false, false);
            assert(inPort->isOpcodeSetting() == false);
            TTAMachine::FUPort* outPort =
                new TTAMachine::FUPort(
                    "out" + Conversion::toString(i), 32, lsUnit, false, false);                
            assert(outPort->isOpcodeSetting() == false);                
        }
        TTAMachine::FUPort* inPort =
            new TTAMachine::FUPort(
               "in_extra" + Conversion::toString(2), 32, lsUnit, false, false);
        assert(inPort->isOpcodeSetting() == false);
        TTAMachine::FUPort* outPort =
            new TTAMachine::FUPort(
               "out_extra" + Conversion::toString(1), 32, lsUnit, false, false);                
        assert(outPort->isOpcodeSetting() == false);                
        
        // Adds aditional read and write port for use with scalar load
        // and stores from the extras
        
        OperationPool pool;
        OperationIndex& index = pool.index();    
        for (int m = 0; m < index.moduleCount(); m++) {        
            OperationModule& module = index.module(m);        
            for (int i = 0; i < index.operationCount(module); i++) {
                TCEString opName = index.operationName(i, module);            
                Operation& op = pool.operation(opName.c_str());
                // Creates HWOperations for memory access operations with
                // number as suffix. ATM this means vector operation.
                if (op.usesMemory() && 
                    (opName.endsWith("2") ||
                    opName.endsWith("4") ||
                    opName.endsWith("8"))) {
                    if (!lsUnit.hasOperation(opName) && 
                        op.numberOfInputs() <= nodeCount_ +1 &&
                        op.numberOfOutputs() <= nodeCount_) {
                        addOperation(lsUnit, op, false);
                    }
                }
                if (op.usesMemory() && 
                    opName.startsWith("LD") &&
                    op.numberOfInputs() == 1 &&
                    op.numberOfOutputs() == 1 &&
                    !lsUnit.hasOperation(opName)) {
                    addOperation(lsUnit, op, true);                    
                }
                if (op.usesMemory() && 
                    opName.startsWith("ST") &&
                    op.numberOfInputs() == 2 &&
                    op.numberOfOutputs() == 0 &&
                    !lsUnit.hasOperation(opName)) {                
                    addOperation(lsUnit, op, true);                    
                }                
            }
        }
        
    }
};

// parameters
const TCEString VectorLSGenerator::NodeCountPN_("node_count");
const TCEString VectorLSGenerator::AddressSpacesPN_("address_spaces");

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(VectorLSGenerator)
