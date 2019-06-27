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
 * @file NumberControl.hh
 *
 * Declaration of NumberControl class.
 *
 * @author Veli-Pekka J��skel�inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_NUMBER_CONTROL_HH
#define TTA_NUMBER_CONTROL_HH

#include <wx/wx.h>

#include "Exception.hh"
#include "FocusTrackingTextCtrl.hh"

/**
 * Text field widget for numeric value input.
 *
 * The widget value can be modified in hexadecimal, binary, int, unsigned int
 * and floating point format. The widget value is always the same four
 * bytes of memory, and the bit pattern is interpreted depending on the
 * selected mode. Text field mode can be selected using an optional
 * mode choicer. Available modes for the choicer are selected by passing the
 * following style flags to the constructor:
 * MODE_HEXADECIMAL, MODE_BINARY, MODE_INT, MODE_UNSIGNED, MODE_FLOAT and
 * MODE_DOUBLE
 *
 * Note that the extra bits in the double value is ignored when the mode
 * is changed from double to any of the other modes.
 *
 * The mode choicer widget can be disabled by using NO_MODE_CHOICER style flag.
 */
class NumberControl : public wxPanel {
public:
    NumberControl(
        wxWindow* parent,
        wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = (MODE_HEXADECIMAL | MODE_BINARY |
                      MODE_INT | MODE_UNSIGNED | MODE_FLOAT),
        int initial = 0,
        const wxString& name = _T("NumberControl"));
    virtual ~NumberControl();

    int intValue() const;
    unsigned int unsignedValue() const;
    float floatValue() const;
    double doubleValue() const;

    long mode() const;

    void setBinMode();
    void setHexMode();
    void setIntMode();
    void setUnsignedMode();
    void setFloatMode();
    void setDoubleMode();

    void setValue(const unsigned int value);
    void setValue(const int value);
    void setValue(const float value);
    void setValue(const double value);

    /// Style flag for binary mode availability.
    static const long MODE_BINARY;
    /// Style flag for signed integer mode availablity.
    static const long MODE_INT;
    /// Style flag for unsigned integer mode availability.
    static const long MODE_UNSIGNED;
    /// Style flag for hexadecimal mode availability.
    static const long MODE_HEXADECIMAL;
    /// Style flag for float mode availability.
    static const long MODE_FLOAT;
    /// Style flag for double mode availability.
    static const long MODE_DOUBLE;
    /// Style flag for base choicer visibility.
    static const long NO_MODE_CHOICER;

private:
    void create(const wxSize& size);
    void update();
    void onModeChoice(wxCommandEvent& event);
    void onText(wxCommandEvent& event);

    /// Text field widget.
    FocusTrackingTextCtrl* text_;
    /// Mode choicer widget.
    wxChoice* modeChoice_;

    /// Current style flags of the widget.
    long style_;

    union Value {
        unsigned int unsignedValue;
        int intValue;
        float floatValue;
        double doubleValue;
    };

    /// Current value of the widget.
    Value value_;

    /// Dummy value string for the validators.
    wxString stringValue_;

    /// Current mode of the widget.
    long mode_;

    /// Choicer item string for the binary mode.
    static const wxString MODE_STRING_BIN;
    /// Choicer item string for the hexadecimal mode.
    static const wxString MODE_STRING_HEX;
    /// Choicer item string for the integer mode.
    static const wxString MODE_STRING_INT;
    /// Choicer item string for the unsigned mode.
    static const wxString MODE_STRING_UNSIGNED;
    /// Choicer item string for the float mode.
    static const wxString MODE_STRING_FLOAT;
    /// Choicer item string for the double mode.
    static const wxString MODE_STRING_DOUBLE;
    /// Mode choicer width.
    static const int CHOICER_WIDTH;

    /// Binary input validator.
    wxTextValidator* binValidator_;
    /// Hexadecimal input validator.
    wxTextValidator* hexValidator_;
    /// Signed integer input validator.
    wxTextValidator* intValidator_;
    /// Unsigned integer input validator.
    wxTextValidator* unsignedValidator_;
    /// Float input validator.
    wxTextValidator* floatValidator_;

    /// IDs for the subwidgets.
    enum {
        ID_BASE = 20000,
        ID_TEXT
    };

    DECLARE_EVENT_TABLE()
};

#endif
