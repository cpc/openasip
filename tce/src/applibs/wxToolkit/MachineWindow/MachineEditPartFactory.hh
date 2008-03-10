/**
 * @file MachineEditPartFactory.hh
 *
 * Declaration of MachineEditPartFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
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
