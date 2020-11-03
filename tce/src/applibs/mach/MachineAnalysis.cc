/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file MachineAnalysis.cc
 *
 * Implementation of MachineAnalysis class.
 *
 * @author Heikki Kultala 2008 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#include "MachineAnalysis.hh"
#include "Machine.hh"

#include "MachineConnectivityCheck.hh"

#include <cmath>

//#define DEBUG_MACHINE_ANALYSIS

/**
 * Constructor.
 * @param machine to analyse.
 */
MachineAnalysis::MachineAnalysis(const TTAMachine::Machine& machine) {

    // do heurictics for FU's

    TTAMachine::Machine::FunctionUnitNavigator fuNav = 
        machine.functionUnitNavigator();

    // todo: better heuristics here, based on available operations etc.
    fuILP_ = fuNav.count() * 0.55;

    // Connectivity heuristics

    int fuPortConnecteds = 0;
    int fuPortUnconnecteds = 0;

    for (int i = 0; i < fuNav.count(); i++) {
        TTAMachine::FunctionUnit& fu1 = *fuNav.item(i);
        for (int j = 0; j < fu1.portCount(); j++ ) {
            TTAMachine::Port& port1 = *fu1.port(j);
            if (port1.inputSocket() != NULL) {
                for (int k = 0; k < fuNav.count(); k++) {
                    TTAMachine::FunctionUnit& fu2 = *fuNav.item(k);
                    for (int l = 0; l < fu2.portCount(); l++ ) {
                        TTAMachine::Port& port2 = *fu2.port(l);
                        if (port2.outputSocket() != NULL) {
                            if (MachineConnectivityCheck::isConnected(
                                    port2,port1)) {
                                fuPortConnecteds++;
                            } else {
                                fuPortUnconnecteds++;
                            }
                        }
                    }
                }
            }
        }
    }
        
    bypassability_ = fuPortConnecteds/
        float(fuPortConnecteds+fuPortUnconnecteds);

    TTAMachine::Machine::RegisterFileNavigator regNav =
        machine.registerFileNavigator();
    int rfCount = regNav.count();

    TTAMachine::Machine::BusNavigator busNav =
        machine.busNavigator();

    int rfFuConnecteds = 0;
    int rfFuUnconnecteds = 0;

    for (int i = 0; i < fuNav.count(); i++) {
        TTAMachine::FunctionUnit& fu1 = *fuNav.item(i);
        for (int j = 0; j < fu1.portCount(); j++ ) {
            TTAMachine::Port& port = *fu1.port(j);
            if (port.inputSocket() != NULL) {
                for (int i = 0; i < rfCount; i++) {
                    TTAMachine::RegisterFile& rf = *regNav.item(i);
                    if (MachineConnectivityCheck::isConnected(rf, port)) {
                        rfFuConnecteds++;
                    } else {
                        rfFuUnconnecteds++;
                    }
                }
            }

            if (port.outputSocket() != NULL) {
                for (int i = 0; i < rfCount; i++) {
                    TTAMachine::RegisterFile& rf = *regNav.item(i);
                    if (MachineConnectivityCheck::isConnected(port,rf)) {
                        rfFuConnecteds++;
                    } else {
                        rfFuUnconnecteds++;
                    }
                }
            }
        }
    }

    // do heuristics for RF's
    
    int rfWritePorts = 0;
    int rfReadPorts = 0;
    for (int i = 0; i < rfCount; i++) {
        TTAMachine::RegisterFile& rf = *regNav.item(i);
        for (int j = 0; j < rf.portCount(); j++) {
            const TTAMachine::Port& port = *rf.port(j);
            if (port.outputSocket() != NULL) {
                rfReadPorts++;
            }
            if (port.inputSocket() != NULL) {
                rfWritePorts++;
            }
        }
    }
    
    // rfILP comes from all RF's with 
    float avgRfWrites = float(rfWritePorts)/pow(rfCount,0.5);
    float avgRfReads = float(rfReadPorts)/pow(rfCount,0.5);
    float bypassedRfReadILP = avgRfReads/(1.17-(0.56*bypassability_));
    float bypassedRfWriteILP = avgRfWrites/(0.83-(0.415*bypassability_));
    float rfReadILP = avgRfReads/1.17;
    float rfWriteILP = avgRfWrites/0.83;

    // todo: fuzzier heuristic
    rfILP_ = std::min(rfReadILP, rfWriteILP);
    bypassedRfILP_ = std::min(bypassedRfReadILP, bypassedRfWriteILP);

    connectivity_ = rfFuConnecteds/
        float(rfFuConnecteds+rfFuUnconnecteds);
    float transfers = connectivity_* busNav.count();
    
    busILP_ = transfers / (2.5-(0.415*bypassability_));

    // take the final value by inverse of pythagoral of inverses
    averageILP_ = pow((pow(busILP_,-2)+
                       pow(bypassedRfILP_,-2)+
                       pow(fuILP_,-2)), -0.5);

    // then calculate guardability.

    float guardedBuses = 0;
    for (int i = 0; i < busNav.count(); i++) {
        TTAMachine::Bus& bus = *busNav.item(i);
        if (bus.guardCount() > 0) {
            guardedBuses++;
        }
    }
    guardability_ = busNav.count() / guardedBuses;

#ifdef DEBUG_MACHINE_ANALYSIS
    std::cout << "Machine analysis done.." << std::endl;
    std::cout << "\tbypassability: " << bypassability_ << std::endl;
    std::cout << "\tconnectivity: " << connectivity_ << std::endl;
    std::cout << "\tguardability: " << guardability_ << std::endl;
    std::cout << "\tRF ilp: " << rfILP_ << std::endl;
    std::cout << "\t\tread ilp: " << rfReadILP << std::endl;
    std::cout << "\t\twrite ilp: " << rfWriteILP << std::endl;
    std::cout << "\tbypassd rf ilp: " << bypassedRfILP_ << std::endl;
    std::cout << "\tbus ilp: " << busILP_ << std::endl;
    std::cout << "\tfu ilp: " << fuILP_ << std::endl;
    std::cout << "total ilp: " << averageILP_ << std::endl;
#endif

}
