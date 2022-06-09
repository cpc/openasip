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
 * @file MachineEditPartFactory.hh
 *
 * Declaration of MachineEditPartFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 13 2004 by vpj, ll, jn, am
 */

#ifndef TTA_MACHINE_EDIT_PART_FACTORY_HH
#define TTA_MACHINE_EDIT_PART_FACTORY_HH

#include <vector>

#include "EditPartFactory.hh"

class EditPart;
class ConnectionEditPart;

namespace TTAMachine {
    class MachinePart;
    class Machine;
    class Bridge;
    class Bus;
}

/**
 * Factory for creating EditParts from any Machine component or the
 * Machine itself.
 */
class MachineEditPartFactory : public EditPartFactory {
public:
    explicit MachineEditPartFactory(EditPolicyFactory& editPolicyFactory);
    virtual ~MachineEditPartFactory();

    virtual EditPart* createEditPart(TTAMachine::MachinePart* component);
    EditPart* createEditPart(TTAMachine::Machine* machine);

private:
    /// Assignment not allowed.
    MachineEditPartFactory& operator=(MachineEditPartFactory& old);
    /// Copying not allowed.
    MachineEditPartFactory(MachineEditPartFactory& old);

    EditPart* getUnits(TTAMachine::Machine* machine);
    EditPart* getSockets(TTAMachine::Machine* machine);
    EditPart* getBusChains(TTAMachine::Machine* machine);
    ConnectionEditPart* createBiDirBridge(ConnectionEditPart* bridge,
					  TTAMachine::Bridge* opposite);
    void findConnectedBridges(ConnectionEditPart*& bridgeEditPart,
			      ConnectionEditPart*& biDirBridge,
			      std::vector<TTAMachine::Bridge*>& bridges,
			      TTAMachine::Bus* bus);
    bool connected(
	const TTAMachine::Bridge* bridge,
	const TTAMachine::Bus* bus1,
	const TTAMachine::Bus* bus2);
};

#endif
