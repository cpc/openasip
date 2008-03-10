/**
 * @file DisasmExecPercentageAttrProvider.hh
 *
 * Declaration DisasmExecPercentageAttrProvider class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASM_EXEC_PERCENTAGE_ATTR_PROVIDER_HH
#define TTA_DISASM_EXEC_PERCENTAGE_ATTR_PROVIDER_HH

#include "ProximDisasmAttrProvider.hh"

class TracedSimulatorFrontend;

/**
 * Proxim disassembly attribute provider, which highlights moves based
 * on execution count ratio to the simulated cycle count.
 */
class DisasmExecPercentageAttrProvider : public ProximDisasmAttrProvider {
public:
    DisasmExecPercentageAttrProvider(const TracedSimulatorFrontend& simulator);
    virtual ~DisasmExecPercentageAttrProvider();
    virtual wxGridCellAttr* moveCellAttr(
        InstructionAddress address, int move);

private:
    const TracedSimulatorFrontend& simulator_;
};

#endif
