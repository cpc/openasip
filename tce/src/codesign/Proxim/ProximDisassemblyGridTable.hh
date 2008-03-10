/**
 * @file ProximDisassemblyGridTable.hh
 *
 * Declaration of ProximDisassemblyGridTable class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_DISASSEMBLY_GRID_TABLE_HH
#define TTA_PROXIM_DISASSEMBLY_GRID_TABLE_HH

#include "DisassemblyGridTable.hh"

class StopPointManager;
class InstructionMemory;
class ProximDisasmAttrProvider;

/**
 * A specialized DisassemblyGridTable class, which adds a column with
 * breakpoint labels and arrow displaying the  current instruction
 * to the disassembly grid.
 */
class ProximDisassemblyGridTable : public DisassemblyGridTable {
public:
    ProximDisassemblyGridTable();
    ~ProximDisassemblyGridTable();

    virtual wxString GetValue(int row, int col);
    virtual int GetNumberCols();
    virtual int GetNumberRows();
    virtual wxString GetColLabelValue(int col);
    virtual wxGridCellAttr* GetAttr(
        int row, int col, wxGridCellAttr::wxAttrKind  kind);

    void setStopPointManager(StopPointManager& manager);
    void setCurrentInstruction(Word address);
    void setMoveCellAttrProvider(ProximDisasmAttrProvider* attrProvider); 
    ProximDisasmAttrProvider* moveCellAttrProvider() const;
    void showPCArrow();
    void hidePCArrow();

private:
    /// Program counter value.
    Word currentInstruction_;
    /// Stoppoint manager managing program breakpoints.
    StopPointManager* bpManager_;
    /// True, if the current istrcution arrow should be displayed.
    bool showPCArrow_;
    ///
    ProximDisasmAttrProvider* attrProvider_;
};

#endif
