#include "BFEarlyGuardBypass.hh"
#include "DataDependenceGraph.hh"
#include "Operation.hh"
#include "Move.hh"
#include "MoveGuard.hh"
#include "MachineConnectivityCheck.hh"
#include "Bus.hh"
#include "Guard.hh"
#include "Terminal.hh"
#include "FUPort.hh"

bool BFEarlyGuardBypass::operator()() { 

    src_ = ddg().onlyRegisterRawSource(mn_, 1);
    if (!src_) {
        return false;
    }
    ProgramOperationPtr po = src_->sourceOperationPtr();
    const Operation& op = po->operation();
    
    auto dPorts = MachineConnectivityCheck::findPossibleDestinationPorts(
        targetMachine(), mn_);
        
    // TODO: this is not deterministic as this uses pointer as the key!
    std::set<const TTAMachine::Bus*> buses;
    
    for (auto p: dPorts) {
        MachineConnectivityCheck::appendConnectedSourceBuses(*p, buses);
    }
    
    for (auto bus: buses) {
        for (int i = 0; i < bus->guardCount(); i++) {
            const TTAMachine::Guard* guard = bus->guard(i);
            const TTAMachine::PortGuard* pg = 
                dynamic_cast<const TTAMachine::PortGuard*>(guard);
            if (pg == NULL || pg->isInverted() != mn_.move().guard().isInverted()) {
                continue;
            }
            const TTAMachine::FUPort* port = pg->port();
            const TTAMachine::FunctionUnit* gfu = port->parentUnit();

            if (gfu->hasOperation(op.name()) &&
                po->outputIndexFromGuard(*pg) ==
                src_->move().source().operationIndex()) {

                if (!po->isLegalFU(*gfu)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                    std::cerr << "\t\t\tFU: " << gfu->name() << " not allowed for operation: " << po->toString() << std::endl;
#endif
                    continue;
                }

                originalGuard_ = &mn_.move().guard().guard();
                // TODO: backup old guard.
                mn_.move().setGuard(new TTAProgram::MoveGuard(*guard));
                ddg().guardConverted(*src_, mn_);
                return true;
            }
        }
    }
    return false;
}

void BFEarlyGuardBypass::undoOnlyMe() {
    ddg().guardRestored(*src_, mn_);    
    mn_.move().setGuard(new TTAProgram::MoveGuard(*originalGuard_));
}
