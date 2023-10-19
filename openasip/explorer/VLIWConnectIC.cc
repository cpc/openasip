/*
    Copyright (c) 2002-2017 Tampere University.

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
 * @file VLIWConnectIC.cc
 *
 * Explorer plugin that adds maximum connections to all the buses in
 * the machine.
 *
 * @author Timo Viitanen 2014
 * @author Alex Hirvonen 2017 (alex.hirvonen-no.spam-gmail.com)
 * @note rating: red
 */

#include <algorithm>
#include <vector>

#include "ControlUnit.hh"
#include "Conversion.hh"
#include "DSDBManager.hh"
#include "DesignSpaceExplorerPlugin.hh"
#include "Exception.hh"
#include "ExplorationTools.hh"
#include "Guard.hh"
#include "HDBRegistry.hh"
#include "Machine.hh"
#include "MachineConnectivityCheck.hh"
#include "Segment.hh"

//using namespace TTAProgram;
using namespace TTAMachine;
using namespace HDB;
using std::endl;

class 
VLIWConnectIC : public DesignSpaceExplorerPlugin {
    PLUGIN_DESCRIPTION("Arranges architecture FUs into a VLIW-like "
        "interconnection by adding separate RF for each distinct bus width.");

    VLIWConnectIC(): DesignSpaceExplorerPlugin(),
        wipeRegisterFile_(false),
        shortImmediateWidth_(6),
        longImmediateBusCount_(1) {
        addParameter(shortImmediateWidthPN_, INT, false, "6",
                     "Short immediate width for each bus.");
        addParameter(longImmediateBusCountPN_, INT, false, "1",
                     "Number of dummy buses for long immediates. (power-of-2)");
        addParameter(wipeRegisterFilePN_, BOOL, false, "true",
                     "Generate a VLIW-style register file.");
    }

    virtual bool requiresStartingPointArchitecture() const { return true; }
    virtual bool producesArchitecture() const { return true; }
    virtual bool requiresHDB() const { return false; }
    virtual bool requiresSimulationData() const { return false; }
    virtual bool requiresApplication() const { return false; }


    /**
     * Arranges architecture FUs into a VLIW-like interconnection. This is
     * typically as baseline for running the BusMergeMinimizer and
     * RFPortMergeMinimizer plugins.
     */
    virtual std::vector<RowID>
    explore(const RowID& configurationID, const unsigned int&) {
        readParameters();

        std::vector<RowID> result;

        DSDBManager& dsdb = db();
        DSDBManager::MachineConfiguration conf;
        conf.hasImplementation = false;
        TTAMachine::Machine* mach = NULL;

        // load the adf from file or from dsdb
        try {
            conf = dsdb.configuration(configurationID);
            mach = dsdb.architecture(conf.architectureID);
        } catch (const Exception& e) {
            std::ostringstream msg(std::ostringstream::out);
            Application::errorStream()
                << "Error loading the adf." << std::endl;
            return result;
        }
        assert(mach != NULL);

        //Find BOOL
        TTAMachine::Machine::RegisterFileNavigator rfNavi 
            = mach->registerFileNavigator();
            
        RegisterFile* boolrf = NULL;
        for (int i = 0; i < rfNavi.count(); i++) {
            RegisterFile* rf = rfNavi.item(i);
            if (rf->width() == 1) {
                boolrf = rf;
            }
        }
        assert(boolrf != NULL);

        // Remove register files, except for BOOL
        if (wipeRegisterFile_) {
            for (int i = 0; i < rfNavi.count(); i++) {
                RegisterFile* rf = rfNavi.item(i);
                if (rf->width() != 1) {
                    mach->removeRegisterFile(*rfNavi.item(i));
                    i--;
                }
            }
        }

        // Remove unconnected sockets
        TTAMachine::Machine::SocketNavigator socketNavi 
            = mach->socketNavigator();
        for (int i = 0; i < socketNavi.count(); i++) {
            if (socketNavi.item(i)->portCount() == 0 ) {
                mach->removeSocket(*socketNavi.item(i));
                i--;
            }
        }

        // Wiping buses also destroys socket directions (input/output)
        // and there seems to be no other way to find out whether a port 
        // is input or output.
        // -> Save the directions before wiping the buses

        // get number of all distinct bus widths in the machine first
        std::vector<int>::iterator iter;

        for (int i = 0; i < socketNavi.count(); i++) {
            Socket* sock = socketNavi.item(i);

            assert(sock->portCount() == 1);
            int width = sock->port(0)->width();

            // combine <=32 bit widths into one cluster
            if (width < 32) {
                width = 32;
            }

            // add non-existing width to distinctBusWidths
            iter = std::find(distinctBusWidths_.begin(),
                distinctBusWidths_.end(), width);
            if (iter == distinctBusWidths_.end()) {
                distinctBusWidths_.push_back(width);
            }
        }

        sort(distinctBusWidths_.begin(), distinctBusWidths_.end());
        int numDistinctBusWidths = distinctBusWidths_.size();

        // save directions and sockets
        std::vector<Socket::Direction> directions;
        std::vector< std::vector<int> > readSockets(numDistinctBusWidths),
            writeSockets(numDistinctBusWidths),
            controlSockets(numDistinctBusWidths);

        for (int i = 0; i < socketNavi.count(); i++) {
            Socket* sock = socketNavi.item(i);
            Socket::Direction dir = sock->direction();
            directions.push_back(dir);

            assert(sock->portCount() == 1);
            Unit* parentUnit = sock->port(0)->parentUnit();
            int width = sock->port(0)->width();
            int widx = widthIndex(width);

            if (dynamic_cast<BaseRegisterFile*>(parentUnit) == NULL
                && dynamic_cast<ControlUnit*>(parentUnit) == NULL) {
                if (dir == Socket::INPUT) {
                    readSockets[widx].push_back(i);
                }
                else {
                    writeSockets[widx].push_back(i);
                }
            }
            else {
                controlSockets[widx].push_back(i);
            }
            
        }

        // Wipe all existing buses

        TTAMachine::Machine::BusNavigator busNavi = mach->busNavigator();
        for (int i = 0; i < busNavi.count(); i++) {
            mach->removeBus(*busNavi.item(i));
            i--;
        }

        // Add new bus for each fu/gcu socket
        std::vector< std::vector<int> > readBuses(numDistinctBusWidths),
            writeBuses(numDistinctBusWidths);
        int busCount = 0;
        
        for (int widx = 0; widx < numDistinctBusWidths; ++widx) {
            int width = distinctBusWidths_[widx];
            
            if (widx == 0) {
                TTAMachine::Segment* newSegment = createBus(mach, 32);
                
                for (unsigned int i = 0; i < controlSockets[widx].size(); i++) {
                    int idx = controlSockets[widx][i];
                    newSegment->attachSocket(*socketNavi.item(idx));
                    socketNavi.item(idx)->setDirection(directions[idx]);
                }
                readBuses[widx].push_back(busCount);
                writeBuses[widx].push_back(busCount++);
            }

            for (unsigned int i = 0; i < readSockets[widx].size(); i++) {
                int idx = readSockets[widx][i];
                TTAMachine::Segment* newSegment = createBus(mach, width);
                newSegment->attachSocket(*socketNavi.item(idx));
                socketNavi.item(idx)->setDirection(directions[idx]);
                readBuses[widx].push_back(busCount++);
            }

            for (unsigned int i = 0; i < writeSockets[widx].size(); i++) {
                int idx = writeSockets[widx][i];
                TTAMachine::Segment* newSegment = createBus(mach, width);
                newSegment->attachSocket(*socketNavi.item(idx));
                socketNavi.item(idx)->setDirection(directions[idx]);
                writeBuses[widx].push_back(busCount++);
            }
        }

        // Add register file
        for (int widx = 0; widx < numDistinctBusWidths; ++widx) {
            if (readBuses[widx].size() == 0 && writeBuses[widx].size() == 0) {
                continue;
            }

            if (wipeRegisterFile_) {
                int width = distinctBusWidths_[widx];
              
                RegisterFile* rf = new RegisterFile(
                    "RF_" + Conversion::toString(width),
                    512, // high enough to avoid register spills
                    width,
                    readBuses[widx].size(),
                    writeBuses[widx].size(),
                    0,
                    RegisterFile::NORMAL);
                mach->addRegisterFile(*rf);

                // Add RF read ports
                for (unsigned int i = 0; i < readBuses[widx].size(); i++) {
                    TCEString socketName = "R" + Conversion::toString(i) + 
                                           "_" + Conversion::toString(width);
                    Socket* newSocket = new Socket(socketName);
                    mach->addSocket(*newSocket);
                    
                    RFPort* newPort = new RFPort(socketName, *rf);
                    newPort->attachSocket(*newSocket);
                    busNavi.item(readBuses[widx][i])->segment(0)->
                        attachSocket(*newSocket);
                    newSocket->setDirection(Socket::OUTPUT);
                }

                // Add RF write ports
                for (unsigned int i = 0; i < writeBuses[widx].size(); i++) {
                    TCEString socketName = "W" + Conversion::toString(i) + 
                                           "_" + Conversion::toString(width);
                    Socket* newSocket = new Socket(socketName);
                    mach->addSocket(*newSocket);
                    
                    RFPort* newPort = new RFPort(socketName, *rf);
                    newPort->attachSocket(*newSocket);
                    busNavi.item(writeBuses[widx][i])->segment(0)->
                        attachSocket(*newSocket);
                    newSocket->setDirection(Socket::INPUT);
                }
            }
        }

        // Create one RF connection to itself
        for (int rfid = 0; rfid < rfNavi.count(); rfid++) {
            RegisterFile* rf = rfNavi.item(rfid);

            if (!MachineConnectivityCheck::isConnected(
                    *rf->firstReadPort(), *rf->firstWritePort())) {
                for (int busid = 0; busid < busNavi.count(); busid++) {
                    Bus* bus = busNavi.item(busid);
                    if (bus->width() == rf->firstReadPort()->width()) {
                        Socket* inSocket =
                            rf->firstWritePort()->inputSocket();
                        Socket* outSocket =
                            rf->firstReadPort()->outputSocket();
                        if (!bus->isConnectedTo(*inSocket)) {
                            bus->segment(0)->attachSocket(*inSocket);
                        }
                        if (!bus->isConnectedTo(*outSocket)) {
                            bus->segment(0)->attachSocket(*outSocket);
                        }
                        break;
                    }
                }
            }
        }

        // Add bypasses
        for (int widx = 0; widx < numDistinctBusWidths; ++widx) {
            for (unsigned int i = 0; i < writeSockets[widx].size(); i++) {
                Socket* output = socketNavi.item(writeSockets[widx][i]);
                for (unsigned int j = 0; j < readBuses[widx].size(); j++) {
                    Bus* readBus = busNavi.item(readBuses[widx][j]);
                    readBus->segment(0)->attachSocket(*output);
                }
            }
        }

        for (int i = 0; i < busNavi.count(); i++) {
            Bus* bus = busNavi.item(i);
            
            new UnconditionalGuard(false, *bus);
            new RegisterGuard(false, *boolrf, 0, bus);
            new RegisterGuard(true, *boolrf, 0, bus);
            new RegisterGuard(false, *boolrf, 1, bus);
            new RegisterGuard(true, *boolrf, 1, bus);
        }

        for (int i = 0; i < busNavi.count(); i++) {
            Bus* bus = busNavi.item(i);
            bus->setImmediateWidth(shortImmediateWidth_);

            // connect each bus to immediate unit to avoid register files access
            //            ImmediateUnit* immu =
            //            mach->immediateUnitNavigator().item(0); if
            //            (!immu->port(0)->outputSocket()->isConnectedTo(*bus))
            //            {
            //                immu->port(0)->outputSocket()->attachBus(*bus->segment(0));
            //            }
        }

        // add unconnected long immediate buses
        if (mach->immediateUnitNavigator().count() != 0) {
            ImmediateUnit* immu = mach->immediateUnitNavigator().item(0);
            
            while (mach->instructionTemplateNavigator().count() > 0) {
                mach->deleteInstructionTemplate(
                    *mach->instructionTemplateNavigator().item(0));
            }

            new InstructionTemplate("no_limm", *mach);

            InstructionTemplate* limm
                = new InstructionTemplate("limm", *mach);
            for (int i = 0; i < longImmediateBusCount_; i++) {
                TTAMachine::Segment* newSegment = createBus(mach, 32);
                Bus* newBus = newSegment->parentBus();

                limm->addSlot(newBus->name(), 32/longImmediateBusCount_, *immu);
            }
        }

        // add machine to configuration
        conf.architectureID = dsdb.addArchitecture(*mach);

        // add new configuration to dsdb
        RowID confID = dsdb.addConfiguration(conf);
        evaluate(conf);
        result.push_back(confID);
        return result;
    }

    protected:

    bool wipeRegisterFile_;
    int shortImmediateWidth_;
    int longImmediateBusCount_;
    // number of distinct bus widths in current machine
    std::vector<int> distinctBusWidths_;
    static const TCEString wipeRegisterFilePN_;
    static const TCEString shortImmediateWidthPN_;
    static const TCEString longImmediateBusCountPN_;


    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        readOptionalParameter(wipeRegisterFilePN_, wipeRegisterFile_);
        readOptionalParameter(shortImmediateWidthPN_, shortImmediateWidth_);
        readOptionalParameter(longImmediateBusCountPN_, longImmediateBusCount_);
    }

    /**
     * Creates a bus with specified bit width.
     *
     * @param mach Machine for that bus.
     * @param width Bit width of a bus.
     * @return Bus segment.
     */
    TTAMachine::Segment* createBus(TTAMachine::Machine* mach, int width) {
        int idx = mach->busNavigator().count();
        TCEString busName = "B" + Conversion::toString(idx);
        Bus* newBus = new Bus(busName, width, 0, Machine::SIGN);
        TTAMachine::Segment* newSegment = 
            new TTAMachine::Segment(busName, *newBus);
        mach->addBus(*newBus);
        return newSegment;
    }

    /**
     * Returns corresponding cluster index in distinct buses vector list.
     *
     * @param width Bus width to get index.
     * @param buses Vector list of distinct buses in current machine.
     * @return Bus width cluster index.
     */
    int widthIndex(int width) {
        if (width < 32) {
            width = 32;
        }

        int i = 0;
        for (auto const& busW : distinctBusWidths_) {
            if (width == busW) {
                break;
            }
            i++;
        }
        return i;
    }

};

const TCEString VLIWConnectIC::wipeRegisterFilePN_("wipe_register_file");
const TCEString VLIWConnectIC::shortImmediateWidthPN_("simm_width");
const TCEString VLIWConnectIC::longImmediateBusCountPN_("limm_bus_count");

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(VLIWConnectIC)

