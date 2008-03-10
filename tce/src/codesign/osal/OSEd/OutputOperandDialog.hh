/**
 * @file OutputOperandDialog.hh
 *
 * Declaration of OutputOperandDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @author Tero Ryynänen 2008 (tero.ryynanen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_OUTPUT_OPERAND_DIALOG_HH
#define TTA_OUTPUT_OPERAND_DIALOG_HH

#include <wx/wx.h>
#include <map>

class Operand;

#include "Operand.hh"

typedef std::map <int, Operand::OperandType> operandList;
typedef std::pair <int, Operand::OperandType> operandPair;

/**
 * Class for adding or modifying output operands.
 */
class OutputOperandDialog : public wxDialog {
public:
    OutputOperandDialog(wxWindow* window, Operand* operand, int index);
    virtual ~OutputOperandDialog();

private:
    /// Copying not allowed.
    OutputOperandDialog(const OutputOperandDialog&);
    /// Assignment not allowed.
    OutputOperandDialog& operator=(const OutputOperandDialog&);

    wxSizer* createContents(wxWindow* window, bool call_fit, bool set_sizer);
    void onOk(wxCommandEvent& event);
    void updateTypes(Operand* operand);
    void setTexts();

    /**
     * Component ids.
     */
    enum {
        ID_MEM_DATA,           ///< Memory data choice.
        ID_OPERATION_OUTPUT_TYPES ///< Output Type ComboBox
    };

    /// Operand to be modified.
    Operand* operand_;
    /// Flag indicating whether operand is memory data or not.
    bool memData_;
    /// Index of the operand.
    int index_;
    /// Choice box for operation input types
    wxChoice* outputTypesComboBox_;
    
    /// Output types
    std::vector<std::string> outputTypes_;
    operandList operandTypes_;

    //Operand::OperandType operandTypes_[4];
    
    DECLARE_EVENT_TABLE()
};

#endif
