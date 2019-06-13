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
 * @file MachineEditPartFactory.cc
 *
 * Definition of MachineEditPartFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 13 2004 by vpj, ll, jn, am
 */

#include <set>

#include "Application.hh"
#include "ContainerTools.hh"
#include "MachineEditPartFactory.hh"
#include "Machine.hh"
#include "FunctionUnit.hh"
#include "ControlUnit.hh"
#include "RegisterFile.hh"
#include "ImmediateUnit.hh"
#include "Port.hh"
#include "Socket.hh"
#include "Bus.hh"
#include "Bridge.hh"
#include "FUFactory.hh"
#include "RFFactory.hh"
#include "IUFactory.hh"
#include "SocketFactory.hh"
#include "BusFactory.hh"
#include "GCUFactory.hh"
#include "BridgeFactory.hh"
#include "EditPart.hh"
#include "ContentsFigure.hh"
#include "BidirBridgeFigure.hh"
#include "UnitContainerFigure.hh"
#include "BusContainerFigure.hh"
#include "SocketContainerFigure.hh"
#include "BusChainFigure.hh"
#include "ConnectionEditPart.hh"

#include "EditPolicyFactory.hh"

using std::vector;
using std::set;
using namespace TTAMachine;

/**
 * The Constructor.
 */
MachineEditPartFactory::MachineEditPartFactory(
    EditPolicyFactory& editPolicyFactory) :
    EditPartFactory(editPolicyFactory) {

    registerFactory(new GCUFactory(editPolicyFactory));
    registerFactory(new FUFactory(editPolicyFactory));
    registerFactory(new IUFactory(editPolicyFactory));
    registerFactory(new RFFactory(editPolicyFactory));
    registerFactory(new BridgeFactory(editPolicyFactory));
    registerFactory(new SocketFactory(editPolicyFactory));
    registerFactory(new BusFactory(editPolicyFactory));
}


/**
 * The Destructor.
 */
MachineEditPartFactory::~MachineEditPartFactory() {
}

/**
 * Delegates to registered factories to create the EditPart.
 *
 * @param component Component of which to create the corresponding
 *        EditPart.
 * @return NULL if none of the factories can create the EditPart.
 */
EditPart*
MachineEditPartFactory::createEditPart(MachinePart* component) {

    EditPart* ep = EditPartFactory::checkCache(component);

    if (ep != NULL) {
	return ep;
    }

    vector<Factory*>::const_iterator i = factories_.begin();

    for (; i != factories_.end(); i++) {
	ep = (*i)->createEditPart(component);
	if (ep != NULL) {
	    EditPartFactory::writeToCache(ep);
	    return ep;
	}
    }

    return NULL;	
}

/**
 * Overloaded implementation for creating the contents EditPart from the
 * machine.
 *
 * @param machine Machine to create the EditPart from.
 * @return The created contents EditPart corresponding to the machine.
 */
EditPart*
MachineEditPartFactory::createEditPart(Machine* machine) {

    EditPart* contents = new EditPart();
    contents->setFigure(new ContentsFigure());
    
    contents->addChild(getUnits(machine));
    contents->addChild(getSockets(machine));
    contents->addChild(getBusChains(machine));

    return contents;
}

/**
 * Navigates through the machine and creates a unit container EditPart.
 *
 * Returns a NULL pointer if there are no units in the given machine.
 *
 * @param machine Machine to navigate through.
 * @return An EditPart containing EditParts for all the units in the
 *         given machine or NULL if there aren't any.
 */
EditPart*
MachineEditPartFactory::getUnits(Machine* machine) {

    Machine::FunctionUnitNavigator fuNav = machine->functionUnitNavigator();
    Machine::RegisterFileNavigator rfNav = machine->registerFileNavigator();
    Machine::ImmediateUnitNavigator iuNav = machine->immediateUnitNavigator();

    ControlUnit* gcu = machine->controlUnit();
    EditPart* units = NULL;

    if (fuNav.count() > 0 || rfNav.count() > 0 || iuNav.count() > 0 ||
	gcu != NULL) {
	
	units = new EditPart();
	units->setFigure(new UnitContainerFigure());
	
	for (int i = 0; i < fuNav.count(); i++) {
	    FunctionUnit* fu = fuNav.item(i);
	    EditPart* ep = createEditPart(fu);
	    assert(ep != NULL);
	    units->addChild(ep);
	}
	
	for (int i = 0; i < rfNav.count(); i++) {
	    RegisterFile* rf = rfNav.item(i);
	    EditPart* ep = createEditPart(rf);
	    assert(ep != NULL);
	    units->addChild(ep);
	}
	
	for (int i = 0; i < iuNav.count(); i++) {
	    ImmediateUnit* iu = iuNav.item(i);
	    EditPart* ep = createEditPart(iu);
	    assert(ep != NULL);
	    units->addChild(ep);
	}

	if (gcu != NULL) {
	    EditPart* ep = createEditPart(gcu);
	    assert(ep != NULL);
	    units->addChild(ep);
	}
    }

    return units;
}

/**
 * Navigates through the machine and creates a socket container EditPart.
 *
 * Returns a NULL pointer if there are no sockets in the given machine.
 *
 * @param machine Machine to navigate through.
 * @return An EditPart containing EditParts for all the sockets in the
 *         given machine or NULL if there aren't any.
 */
EditPart*
MachineEditPartFactory::getSockets(Machine* machine) {

    Machine::SocketNavigator socketNav = machine->socketNavigator();
    EditPart* sockets = NULL;

    if (socketNav.count() > 0) {

	sockets = new EditPart();
	sockets->setFigure(new SocketContainerFigure());
	
	for (int i = 0; i < socketNav.count(); i++) {
	    Socket* socket = socketNav.item(i);
	    EditPart* ep = createEditPart(socket);
	    assert(ep != NULL);
	    sockets->addChild(ep);
	}
    }

    return sockets;
}

/**
 * Navigates through the machine and creates a bus container EditPart.
 *
 * Returns a NULL pointer if there are no buses in the given machine.
 *
 * @param machine Machine to navigate through.
 * @return An EditPart containing EditParts for all the buses and bus
 *         chains in the given machine or NULL if there aren't any.
 */
EditPart*
MachineEditPartFactory::getBusChains(Machine* machine) {
    
    Machine::BusNavigator busNav = machine->busNavigator();
    
    EditPart* busContainer = NULL;
    
    if (busNav.count() > 0) {
	
	set<Bus*> sorted;
	busContainer = new EditPart();
	busContainer->setFigure(new BusContainerFigure());
	vector<Bus*> buses;
	
	for (int i = 0; i < busNav.count(); i++) {
	    buses.push_back(busNav.item(i));
	}
	
	// get bridges
	
	Machine::BridgeNavigator bridgeNav = machine->bridgeNavigator();
	vector<Bridge*> bridges;
	
	for (int i = 0; i < bridgeNav.count(); i++) {
	    bridges.push_back(bridgeNav.item(i));
	}

	// sort the bus chains
	
	for (unsigned int i = 0; i < buses.size(); i++) {
	    
	    if (!ContainerTools::containsValue(sorted, buses[i])) {

		sorted.insert(buses[i]);
		Bus* first = buses[i];

		// browse to the beginning of the chain
		while (first->hasPreviousBus()) {
		    first = first->previousBus();
		}
		
		// start adding buses to the chain

		if (first->hasNextBus()) {

		    EditPart* busChain = new EditPart();
		    busChain->setFigure(new BusChainFigure());

		    while (true) {
			sorted.insert(first);

			// find also the bridge(s) in between
			ConnectionEditPart* bridgeEditPart = NULL;
			ConnectionEditPart* biDirBridge = NULL;

			if (first->hasNextBus()) {
			    findConnectedBridges(
				bridgeEditPart, biDirBridge, bridges, first);
			}

			// add the bus and the following bridge to the chain

			EditPart* nextEditPart = createEditPart(first);
			assert(nextEditPart != NULL);
			busChain->addChild(nextEditPart);

			if (biDirBridge != NULL) {
			    busChain->addChild(biDirBridge);
			} else if (bridgeEditPart != NULL) {
			    busChain->addChild(bridgeEditPart);
			}

			if (first->hasNextBus()) {
			    first = first->nextBus();
			} else {
			    break;
			}
		    }

		    busContainer->addChild(busChain);

		} else {
		    // add a single bus
		    EditPart* busEditPart = createEditPart(buses[i]);
		    assert(busEditPart != NULL);
		    busContainer->addChild(busEditPart);
		}
	    }
	}
    }

    return busContainer;
}

/**
 * Creates an EditPart for a bidirectional bridge.
 *
 * @param bridge EditPart representing one of the bridges.
 * @param opposite The opposite bridge.
 * @return EditPart for the bidirectional bridge.
 */
ConnectionEditPart*
MachineEditPartFactory::createBiDirBridge(
    ConnectionEditPart* bridge,
    Bridge* opposite) {

    ConnectionEditPart* biDirBridge = new ConnectionEditPart();
    biDirBridge->setSelectable(true);
    BidirBridgeFigure* fig = new BidirBridgeFigure();
    fig->setDirection(BridgeFigure::DIR_BIDIR);
    biDirBridge->setFigure(fig);
    EditPart* oppositeBridge = createEditPart(opposite);
    bridge->setSelectable(false);
    oppositeBridge->setSelectable(false);

    biDirBridge->setModel(bridge->model());
    biDirBridge->setSource(bridge->source());
    biDirBridge->setTarget(bridge->target());

    biDirBridge->installEditPolicy(
	editPolicyFactory_.createBridgeEditPolicy());

    biDirBridge->addChild(bridge);
    biDirBridge->addChild(oppositeBridge);

    return biDirBridge;
}

/**
 * Finds and returns the bridge or bridges that are connected to a bus.
 *
 * @param bridgeEditPart Where to store one bridge, no change if no
          bridge found.
 * @param biDirBridge Where to store the other bridge, no change if no other
 *        bridge found.
 * @param bridges Which bridges to search.
 * @param bus Bus to which the searched bridges should be connected.
 */
void
MachineEditPartFactory::findConnectedBridges(
    ConnectionEditPart*& bridgeEditPart,
    ConnectionEditPart*& biDirBridge,
    vector<Bridge*>& bridges,
    Bus* bus) {
    
    unsigned int j = 0;
    
    while (j < bridges.size()) {
	if (connected(bridges[j], bus, bus->nextBus())) {
	    bridgeEditPart = dynamic_cast<ConnectionEditPart*>(
		createEditPart(bridges[j]));
	    break;
	}
	j++;
    }
    
    // check if the bridge is bidirectional
    j++;
    while (j < bridges.size()) {
	if (connected(bridges[j], bus, bus->nextBus())) {
	    biDirBridge =
		createBiDirBridge(bridgeEditPart, bridges[j]);
	    break;
	}
	j++;
    }
}

/**
 * Tells whether a bridge is connected to two specific buses.
 *
 * @param bridge The bridge.
 * @param bus1 One bus.
 * @param bus2 The other bus.
 * @return True if the bridge is connected to the buses, false otherwise.
 */
bool
MachineEditPartFactory::connected(
    const Bridge* bridge,
    const Bus* bus1,
    const Bus* bus2) {

    return (bridge->sourceBus() == bus1 &&
    bridge->destinationBus() == bus2) ||
    (bridge->destinationBus() == bus1 &&
    bridge->sourceBus() == bus2);
}
