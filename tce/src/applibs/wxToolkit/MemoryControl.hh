/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file MemoryControl.hh
 *
 * Declaration of MemoryControl class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MEMORY_CONTROL_HH
#define TTA_MEMORY_CONTROL_HH

#include <wx/wx.h>
#include <wx/grid.h>
#include <vector>

#include "BaseType.hh"
#include "Exception.hh"

class MemoryGridTable;
class Memory;

/**
 * Widget for showing the contents of the memory.
 *
 * The contents of the memory is shown in bytes, half words, words, or
 * double words, and either in binary, hexa, float, decimal, or double format.
 */
class MemoryControl : public wxPanel {
public:
    MemoryControl(
        wxWindow* parent,
        Memory* memory,
        wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        const wxString& name = _T("MemoryControl"));

    virtual ~MemoryControl();
    void updateView();
    void setMemory(Memory* memory);

    void clearHighlights();
    void highlight(Word address, unsigned count, const wxColour& colour)
        throw (OutOfRange);

private:

    /// Copying not allowed.
    MemoryControl(const MemoryControl&);
    /// Assignment not allowed.
    MemoryControl& operator=(const MemoryControl&);

    /// Size label for byte size.
    static const std::string SIZE_MAU;
    /// Size label for half word size.
    static const std::string SIZE_TWO_MAUS;
    /// Size label for word size.
    static const std::string SIZE_FOUR_MAUS;
    /// Size label for word size.
    static const std::string SIZE_EIGHT_MAUS;

    /// Data label for binary format.
    static const std::string DATA_BIN;
    /// Data label for hexa format.
    static const std::string DATA_HEX;
    /// Data label for signed int format.
    static const std::string DATA_SIGNED_INT;
    /// Data label for unsigned int format.
    static const std::string DATA_UNSIGNED_INT;
    /// Data label for float format.
    static const std::string DATA_FLOAT;
    /// Data label for double format.
    static const std::string DATA_DOUBLE;

    /// Table width label for 8 column mode
    static const wxString WIDTH_8;
    /// Table width label for 16 column mode
    static const wxString WIDTH_16;
    /// Table width label for 32 column mode
    static const wxString WIDTH_32;

    void createContents();
    void onGoTo(wxCommandEvent& event);
    void onSizeModeChanged(wxCommandEvent&);
    void onDataModeChanged(wxCommandEvent&);
    void onWidthChanged(wxCommandEvent&);
    void onSize(wxSizeEvent&);
    void onWriteMemory(wxGridEvent& event);
    void onChar(wxKeyEvent& event);
    void clearMemory();
    void copySelection();

    /// Used for access to memory contents.
    Memory* memory_;
    /// Size of the minimum addressable unit.
    int MAUSize_;
    /// Start point of memory.
    Word start_;
    /// End point of memory.
    Word end_;
    /// Grid in which the contents of the memory is written.
    wxGrid* grid_;
    /// Mode of the data in the cells.
    wxChoice* dataMode_;
    /// Mode of the data size.
    wxChoice* sizeMode_;
    /// Grid width choicer.
    wxChoice* widthMode_;
    /// Go to address.
    wxString goToAddress_;
    /// Grid contents.
    MemoryGridTable* table_;
    /// Top level sizer of the window.
    wxBoxSizer* sizer_;
    /// Number of maus displayed in a cell.
    unsigned mausPerCell_;

    std::vector<unsigned> highlights_;
    /**
     * Widget ids.
     */
    enum {
        ID_GRID,
        ID_CHOICE_MODE,
        ID_CHOICE_DATA,
	ID_CHOICE_WIDTH,
        ID_ADDRESS_GO_TO,
        ID_BUTTON_GO_TO,
        ID_SPIN_SIZE
    };

    DECLARE_EVENT_TABLE()
};

#endif
