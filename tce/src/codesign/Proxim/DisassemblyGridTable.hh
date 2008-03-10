/**
 * @file DisassemblyGridTable.hh
 *
 * Declaration of DisassemblyGridTable class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_GRID_TABLE_HH
#define TTA_DISASSEMBLY_GRID_TABLE_HH

#include <map>
#include <string>
#include <wx/wx.h>
#include <wx/grid.h>
#include "BaseType.hh"

namespace TTAProgram {
    class Program;
}

class POMDisassembler;

/**
 * Grid table class which allows wxGrid to display disassembly of a tta program
 * by disassembling only the currently visible instructions. Purpose of the
 * 'lazy' disasembling is to conserve memory and processing time when the
 * program is very large.
 */
class DisassemblyGridTable : public wxGridTableBase {
public:
    DisassemblyGridTable();
    virtual ~DisassemblyGridTable();
    virtual int GetNumberRows();
    virtual int GetNumberCols();
    virtual bool IsEmptyCell(int row, int col);
    virtual wxString GetValue(int row, int col);
    virtual wxString GetRowLabelValue(int row);
    virtual wxString GetColLabelValue(int col);
    virtual void SetValue(int row, int col, const wxString& value);
    virtual bool CanHaveAttributes();
    int rowOfAddress(Word address);
    Word addressOfRow(int row);
    void loadProgram(const TTAProgram::Program& program);
protected:
    /// Program loaded in the table.
    const TTAProgram::Program* program_;
private:
    /// Disassembler for generating instruction disassemblies.
    POMDisassembler*  disassembler_;

    /// Type for the labels multimap
    typedef std::multimap<int, std::string> LabelMap;
    
    /// Program labels.
    LabelMap labels_;
};

#endif
