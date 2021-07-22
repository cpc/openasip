/*
    Copyright (c) 2002-2021 Tampere University.

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
 * @file BlocksConnectIC.cc (Derived from VLIWConnectIC.cc)
 *
 * Explorer plugin, that generates connections for Blocks machine
 *
 * @author Kanishkan Vadivel 2021
 * @note rating: red
 */

#include <algorithm>
#include <vector>

#include "ControlUnit.hh"
#include "Conversion.hh"
#include "DSDBManager.hh"
#include "DesignSpaceExplorerPlugin.hh"
#include "Exception.hh"
#include "Guard.hh"
#include "HDBRegistry.hh"
#include "Machine.hh"
#include "Segment.hh"

// using namespace TTAProgram;
using namespace TTAMachine;
using namespace HDB;
using std::endl;

class BlocksConnectIC : public DesignSpaceExplorerPlugin {
    PLUGIN_DESCRIPTION(
        "Arranges architecture FUs into a Blocks-like interconnection");

    BlocksConnectIC() : DesignSpaceExplorerPlugin() {}

    virtual bool
    requiresStartingPointArchitecture() const {
        return true;
    }
    virtual bool
    producesArchitecture() const {
        return true;
    }
    virtual bool
    requiresHDB() const {
        return false;
    }
    virtual bool
    requiresSimulationData() const {
        return false;
    }
    virtual bool
    requiresApplication() const {
        return false;
    }

    /**
     *
     * Arranges architecture FUs into a Blocks-like interconnection.
     * This is typically as baseline for running the BusMergeMinimizer and
     * RFPortMergeMinimizer plugins.
     * Reference: Blocks, a reconfigurable architecture combining
     * energy efficiency and flexibility. Wijtvliet, M. (2020).
     * Technische Universiteit Eindhoven.
     */
    virtual std::vector<RowID>
    explore(const RowID& configurationID, const unsigned int&) {
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

        // Remove unconnected sockets
        TTAMachine::Machine::SocketNavigator socketNavi =
            mach->socketNavigator();
        for (int i = 0; i < socketNavi.count(); i++) {
            if (socketNavi.item(i)->portCount() == 0) {
                mach->removeSocket(*socketNavi.item(i));
                i--;
            }
        }

        // Wiping buses also destroys socket directions (input/output)
        // and there seems to be no other way to find out whether a port
        // is input or output.
        // Save the directions before wiping the buses
        for (int i = 0; i < socketNavi.count(); i++) {
            Socket* sock = socketNavi.item(i);
            // Restrict to 32-bit bus
            int width = sock->port(0)->width();
            assert(width == 32 && "ADF has Socket != 32-bit");
        }

        // save directions and sockets
        std::vector<Socket::Direction> directions;
        std::vector<int> readSockets, writeSockets;
        int gcu_ra_input, gcu_ra_output, gcu_pc;

        for (int i = 0; i < socketNavi.count(); i++) {
            Socket* sock = socketNavi.item(i);
            Socket::Direction dir = sock->direction();
            directions.push_back(dir);

            // Allow only 2-output buffers at max
            int port_count = sock->portCount();
            assert(
                port_count > 0 && port_count <= 2 &&
                "Socket is not connected or connected to more than 2 FUs");
            // Separate GCU RA socket for special handling
            Unit* parentUnit = sock->port(0)->parentUnit();
            ControlUnit* gcu = dynamic_cast<ControlUnit*>(parentUnit);
            if (dir == Socket::INPUT) {
                if (gcu != NULL && sock->port(0)->name() == "ra") {
                    gcu_ra_input = i;
                } else {
                    readSockets.push_back(i);
                }
            } else {
                writeSockets.push_back(i);
                if (gcu != NULL && sock->port(0)->name() == "ra") {
                    gcu_ra_output = i;
                }
            }
        }

        // Wipe all existing buses
        TTAMachine::Machine::BusNavigator busNavi = mach->busNavigator();
        for (int i = 0; i < busNavi.count(); i++) {
            mach->removeBus(*busNavi.item(i));
            i--;
        }

        // Add new bus for each input port (i.e. We mux the 4xinput via single
        // bus)
        int busCount = 0;
        std::vector<int> readBuses, writeBuses;

        /* initialize random seed: */
        std::srand(time(NULL));
        for (unsigned int i = 0; i < readSockets.size(); i++) {
            int rd_soc_idx = readSockets[i];
            Socket* input_sock = socketNavi.item(rd_soc_idx);

            // Attach 32-bit bus segment
            TTAMachine::Segment* newSegment = createBus(mach, 32);
            newSegment->attachSocket(*input_sock);
            input_sock->setDirection(directions[rd_soc_idx]);
            // Handle special case for GCU
            Unit* parentUnit = input_sock->port(0)->parentUnit();
            if (dynamic_cast<ControlUnit*>(parentUnit) != NULL &&
                input_sock->port(0)->name() == "pc") {
                Socket* ra_sock = socketNavi.item(gcu_ra_input);
                newSegment->attachSocket(*ra_sock);
                ra_sock->setDirection(directions[gcu_ra_input]);
                gcu_pc = rd_soc_idx;
            }
            readBuses.push_back(busCount++);

            // Make connections to the socket (maximum 4 connections)
            int output_socket_count = writeSockets.size();
            int req_connections = std::min(output_socket_count, 4);
            while (req_connections > 0) {
                // Select random FU output port
                int wr_soc_idx =
                    writeSockets[std::rand() % writeSockets.size()];
                Socket* output_sock = socketNavi.item(wr_soc_idx);

                // Check if we already have connections
                if (newSegment->isConnectedTo(*output_sock)) {
                    continue;
                }

                // No connections to this port yet; make connection
                newSegment->attachSocket(*output_sock);
                output_sock->setDirection(directions[wr_soc_idx]);
                req_connections--;
            }
        }

        // Create instruction template for imm unit
        if (mach->immediateUnitNavigator().count() != 0) {
            ImmediateUnit* immu = mach->immediateUnitNavigator().item(0);

            while (mach->instructionTemplateNavigator().count() > 0) {
                mach->deleteInstructionTemplate(
                    *mach->instructionTemplateNavigator().item(0));
            }

            new InstructionTemplate("no_limm", *mach);

            InstructionTemplate* limm =
                new InstructionTemplate("limm", *mach);
            TTAMachine::Segment* newSegment = createBus(mach, 32);
            Bus* newBus = newSegment->parentBus();

            limm->addSlot(newBus->name(), 32, *immu);

            // Attch GCU's RA and PC sockets (We do not have explicit RA port
            // in Blocks; so, this is just to emulate the effect of single
            // port.)
            newSegment->attachSocket(*socketNavi.item(gcu_ra_input));
            newSegment->attachSocket(*socketNavi.item(gcu_ra_output));
            newSegment->attachSocket(*socketNavi.item(gcu_pc));
        }

        // add machine to configuration
        conf.architectureID = dsdb.addArchitecture(*mach);

        // add new configuration to dsdb
        RowID confID = dsdb.addConfiguration(conf);
        result.push_back(confID);
        return result;
    }

protected:
    /**
     * Creates a bus with specified bit width.
     *
     * @param mach Machine for that bus.
     * @param width Bit width of a bus.
     * @return Bus segment.
     */
    TTAMachine::Segment*
    createBus(TTAMachine::Machine* mach, int width) {
        int idx = mach->busNavigator().count();
        TCEString busName = "B" + Conversion::toString(idx);
        Bus* newBus = new Bus(busName, width, 0, Machine::SIGN);
        TTAMachine::Segment* newSegment =
            new TTAMachine::Segment(busName, *newBus);
        mach->addBus(*newBus);
        return newSegment;
    }
};

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(BlocksConnectIC)
