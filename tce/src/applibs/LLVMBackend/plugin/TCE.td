//
// TCE Universal target machine definition.
//
// Top level tablegen file for TCE target.
//
// Author: Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
//

include "Target.td"

include "GenRegisterInfo.td"
include "TCEInstrInfo.td"

def TCEInstrInfo : InstrInfo {
}

def TCE : Target {
    let InstructionSet = TCEInstrInfo;
}
