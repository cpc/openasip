/**
 * @file ProximDisasmAttrProvider.hh
 *
 * Declaration ProximDisasmAttrProvider class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_DISASM_ATTR_PROVIDER_HH
#define TTA_PROXIM_DISASM_ATTR_PROVIDER_HH

#include "BaseType.hh"

class wxGridCellAttr;

/**
 * Interface for highlighter classes which provide highlighting colours
 * for disassembly grid cells.
 */
class ProximDisasmAttrProvider {
public:
    ProximDisasmAttrProvider();
    virtual ~ProximDisasmAttrProvider();
    virtual void update();
    virtual wxGridCellAttr* moveCellAttr(
        InstructionAddress address, int move) = 0;
};

#endif
