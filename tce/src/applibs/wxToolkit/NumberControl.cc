/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file NumberControl.cc
 *
 * Implementation of NumberControl class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>

#include "Application.hh"
#include "NumberControl.hh"
#include "Conversion.hh"
#include "WxConversion.hh"


using std::string;

// Widget style flags.
const long NumberControl::MODE_BINARY = 1;
const long NumberControl::MODE_HEXADECIMAL = 2;
const long NumberControl::MODE_INT = 4;
const long NumberControl::MODE_UNSIGNED = 8;
const long NumberControl::MODE_FLOAT = 16;
const long NumberControl::MODE_DOUBLE = 32;
const long NumberControl::NO_MODE_CHOICER = 64;

// String labels for the mode choicer.
const wxString NumberControl::MODE_STRING_BIN = _T("bin");
const wxString NumberControl::MODE_STRING_HEX = _T("hex");
const wxString NumberControl::MODE_STRING_INT = _T("int");
const wxString NumberControl::MODE_STRING_UNSIGNED = _T("unsigned");
const wxString NumberControl::MODE_STRING_FLOAT = _T("float");
const wxString NumberControl::MODE_STRING_DOUBLE = _T("double");

// width of the base choicer
const int NumberControl::CHOICER_WIDTH = 100;

BEGIN_EVENT_TABLE(NumberControl, wxPanel)
    EVT_CHOICE(ID_BASE, NumberControl::onModeChoice)
    EVT_TEXT_ENTER(ID_TEXT, NumberControl::onText)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param window Parent window of the widget.
 * @param id ID of the widget.
 * @param pos Position of the widget.
 * @param size Size of the widget.
 * @param style Style flags for the widget, see class comment for list of
 *              available flags.
 * @param value Initial value of the input.
 * @param name Name identifier for the widget.
 * @exception OutOfRange If the value is out of the range.
 * @exception InvalidData If the minimum value is higher than the maximum.
 */
NumberControl::NumberControl(
    wxWindow* parent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    int initial,
    const wxString& name) :
    wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL, name),
    text_(NULL),
    modeChoice_(NULL),
    style_(style),
    stringValue_(_T("")),
    mode_(0),
    binValidator_(NULL),
    hexValidator_(NULL),
    intValidator_(NULL),
    unsignedValidator_(NULL),
    floatValidator_(NULL) {

    value_.unsignedValue =initial;

    // Create subwidgets.
    create(size);
}


/**
 * The destructor.
 */
NumberControl::~NumberControl() {
    if (binValidator_ != NULL) {
        delete binValidator_;
    }
    if (hexValidator_ != NULL) {
        delete hexValidator_;
    }
    if (intValidator_ != NULL) {
        delete intValidator_;
    }
    if (unsignedValidator_ != NULL) {
        delete unsignedValidator_;
    }
    if (floatValidator_ != NULL) {
        delete floatValidator_;
    }
}


/**
 * Creates the textfield and optional base choicer subwidgets for the control.
 */
void
NumberControl::create(const wxSize& size) {

    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

    int textWidth = size.GetWidth();
    if ((style_ & NO_MODE_CHOICER) != 0) {
        textWidth = textWidth - CHOICER_WIDTH;
        if (textWidth < 20) {
            textWidth = 20;
        }
    }

    // Create text field widget.
    text_ =
        new FocusTrackingTextCtrl(this, ID_TEXT, _T(""), wxDefaultPosition,
                                  wxSize(textWidth, -1));

    sizer->Add(text_, 0, wxALIGN_CENTER_VERTICAL);

    if ((style_ & NO_MODE_CHOICER) == 0) {
        // Create base choicer.
        modeChoice_ = new wxChoice(this, ID_BASE, wxDefaultPosition,
                                   wxSize(CHOICER_WIDTH, -1));

        // append base choices
        if ((style_ & MODE_BINARY) != 0) {
            modeChoice_->Append(MODE_STRING_BIN);
        }
        if ((style_ & MODE_HEXADECIMAL) != 0) {
            modeChoice_->Append(MODE_STRING_HEX);
        }
        if ((style_ & MODE_INT) != 0) {
            modeChoice_->Append(MODE_STRING_INT);
        }
        if ((style_ & MODE_UNSIGNED) != 0) {
            modeChoice_->Append(MODE_STRING_UNSIGNED);
        }
        if ((style_ & MODE_FLOAT) != 0) {
            modeChoice_->Append(MODE_STRING_FLOAT);
        }
        if ((style_ & MODE_DOUBLE) != 0) {
            modeChoice_->Append(MODE_STRING_DOUBLE);
        }

        sizer->Add(modeChoice_, 0, wxALIGN_CENTER_VERTICAL);

        // set initial mode
        if ((style_ & MODE_INT) != 0) {
            setIntMode();
        } else if ((style_ & MODE_UNSIGNED) != 0) {
            setUnsignedMode();
        } else if ((style_ & MODE_FLOAT) != 0) {
            setFloatMode();
        } else if ((style_ & MODE_HEXADECIMAL) != 0) {
            setHexMode();
        } else if ((style_ & MODE_BINARY) != 0) {
            setBinMode();
        } else if ((style_ & MODE_DOUBLE) != 0) {
            setDoubleMode();
        } else {
            assert(false);
        }
    }

    SetSizer(sizer);
    Fit();
}


/**
 * Updates the value on the text field according to the selected mode.
 */
void
NumberControl::update() {

    // binary mode
    if (mode_ == MODE_BINARY) {
        string binString = Conversion::toBinString(value_.intValue);
        // Strip trailing 'b' from the binary string.
        binString = binString.substr(0, binString.length() - 1);
        text_->SetValue(WxConversion::toWxString(binString));
        return;
    }

    // hexadecimal mode
    if (mode_ == MODE_HEXADECIMAL) {
        string hexString = Conversion::toHexString(value_.intValue);
        // Strip '0x' from the begining of the hexstring.
        hexString = hexString.substr(2, hexString.length() - 2);
        text_->SetValue(WxConversion::toWxString(hexString));
        return;
    }

    // int mode
    if (mode_ == MODE_INT) {
        string intString = Conversion::toString(value_.intValue);
        text_->SetValue(WxConversion::toWxString(intString));
        return;
    }

    // unsigned mode
    if (mode_ == MODE_UNSIGNED) {
        string unsignedString = Conversion::toString(value_.unsignedValue);
        text_->SetValue(WxConversion::toWxString(unsignedString));
        return;
    }

    // float mode
    if (mode_ == MODE_FLOAT) {
        string floatString = Conversion::toString(value_.floatValue);
        text_->SetValue(WxConversion::toWxString(floatString));
        return;
    }

    // float mode
    if (mode_ == MODE_DOUBLE) {
        string doubleString = Conversion::toString(value_.doubleValue);
        text_->SetValue(WxConversion::toWxString(doubleString));
        return;
    }

    // Error: no mode selected.
    assert(false);
}


/**
 * Event handler, which validates the text field value and transfers data
 * from the text field to the value variable.
 *
 * This event handler is called when user has input new value to the text
 * field.
 */
void
NumberControl::onText(wxCommandEvent&) {

    string stringValue = WxConversion::toString(text_->GetValue());

    if (stringValue == "") {
        value_.intValue = 0;
        update();
        wxCommandEvent textEvent(wxEVT_COMMAND_TEXT_UPDATED, GetId());
        GetParent()->AddPendingEvent(textEvent);
        return;
    }

    try {
        if (mode_ == MODE_INT) {
            // int mode
            value_.intValue = Conversion::toInt(stringValue);
        } else if (mode_ == MODE_BINARY) {
            // binary mode
            stringValue = stringValue + "b";
            value_.intValue = Conversion::toInt(stringValue);
        } else if (mode_ == MODE_HEXADECIMAL) {
            // hexadecimal mode
            stringValue = "0x" + stringValue;
            value_.intValue = Conversion::toInt(stringValue);
        } else if (mode_ == MODE_UNSIGNED) {
            // unsigned mode
            value_.unsignedValue = Conversion::toUnsignedInt(stringValue);
        } else if (mode_ == MODE_FLOAT) {
            // float mode
            value_.floatValue = Conversion::toFloat(stringValue);
        } else if (mode_ == MODE_DOUBLE) {
            // double mode
            value_.doubleValue = Conversion::toDouble(stringValue);
        } else {
            assert(false);
        }
    } catch (NumberFormatException& e) {
        // invalid input in the text field
        value_.intValue = 0;
    }

    wxCommandEvent textEvent(wxEVT_COMMAND_TEXT_UPDATED, GetId());
    GetParent()->AddPendingEvent(textEvent);

    if (text_->IsModified()) {
        update();
    }
}


/**
 * Event handler for the mode choicer.
 *
 * Updates text field according to the selected mode.
 */
void
NumberControl::onModeChoice(wxCommandEvent&) {

    // Udate the value from the textfield input.
    wxCommandEvent dummy;
    onText(dummy);

    // Set the selected mode.
    if (modeChoice_->GetStringSelection() == MODE_STRING_BIN) {
        setBinMode();
    }else if (modeChoice_->GetStringSelection() == MODE_STRING_HEX) {
        setHexMode();
    } else if (modeChoice_->GetStringSelection() == MODE_STRING_INT) {
        setIntMode();
    } else if (modeChoice_->GetStringSelection() == MODE_STRING_UNSIGNED) {
        setUnsignedMode();
    } else if (modeChoice_->GetStringSelection() == MODE_STRING_FLOAT) {
        setFloatMode();
    } else if (modeChoice_->GetStringSelection() == MODE_STRING_DOUBLE) {
        setDoubleMode();
    } else {
        assert(false);
    }
}


/**
 * Sets the value on the widget.
 *
 * @param value Unsigned value to set.
 */
void
NumberControl::setValue(const int value) {
    value_.intValue = value;
    update();
}


/**
 * Sets the value on the widget.
 *
 * @param value Integer value to set.
 */
void
NumberControl::setValue(const unsigned int value) {
    value_.unsignedValue = value;
    update();
}


/**
 * Sets the value on the widget.
 *
 * @param value Float value to set.
 */
void
NumberControl::setValue(const float value) {
    value_.floatValue = value;
    update();
}


/**
 * Sets the value on the widget.
 *
 * @param value Double value to set.
 */
void
NumberControl::setValue(const double value) {
    value_.doubleValue = value;
    update();
}


/**
 * Returns the current value.
 *
 * @return Current value of the widget as int.
 */
int
NumberControl::intValue() const {
    return value_.intValue;
}


/**
 * Returns the current value.
 *
 * @return Current value of the widget as unsigned int
 */
unsigned int
NumberControl::unsignedValue() const {
    return value_.unsignedValue;
}


/**
 * Returns the current value.
 *
 * @return Current value of the widget as float.
 */
float
NumberControl::floatValue() const {
    return value_.floatValue;
}


/**
 * Returns the current value.
 *
 * @return Current value of the widget as double.
 */
double
NumberControl::doubleValue() const {
    return value_.doubleValue;
}


/**
 * Sets the widget to int mode.
 *
 * @exception NotAvailable If the MODE_INT style was not specified to the
 *                         constructor.
 */
void
NumberControl::setIntMode()
    throw (NotAvailable) {

    // Check that the int mode is available.
    if ((style_ & MODE_INT) == 0) {
        // Int mode not available.
        string procName = "NubmerControl::setIntMode";
        throw NotAvailable(__FILE__, __LINE__, procName);
    }

    // Create decimal input validator if necessary.
    if (intValidator_ == NULL) {
        intValidator_ = new wxTextValidator(wxFILTER_NUMERIC, &stringValue_);
    }

    // Set the decimal validator and update the widget.
    text_->SetValidator(*intValidator_);
    mode_ = MODE_INT;
    if (modeChoice_ != NULL &&
        modeChoice_->GetStringSelection() != MODE_STRING_INT) {

        modeChoice_->SetStringSelection(MODE_STRING_INT);
    }
    update();
}


/**
 * Sets the widget to binary mode.
 *
 * @exception NotAvailable If the MODE_BINARY style was not specified to the
 *                         contructor.
 */
void
NumberControl::setBinMode()
    throw (NotAvailable) {

    // Check that the binary mode is available.
    if ((style_ & MODE_BINARY) == 0) {
        // Binary mode not available.
        string procName = "NubmerControl::setBinMode";
        throw NotAvailable(__FILE__, __LINE__, procName);
    }

    // Create binary input validator if necessary.
    if (binValidator_ == NULL) {
        binValidator_ =
            new wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST, &stringValue_);

#if wxCHECK_VERSION(2, 5, 4)
        wxArrayString includes;
        includes.Add(_T("0"));
        includes.Add(_T("1"));
        binValidator_->SetIncludes(includes);
#else
        binValidator_->SetIncludeList(
            wxStringList(_T("0"), _T("1"), NULL));
#endif

    }

    // Set the binary validator and update the widget.
    text_->SetValidator(*binValidator_);
    mode_ = MODE_BINARY;
    if (modeChoice_ != NULL &&
        modeChoice_->GetStringSelection() != MODE_STRING_BIN) {

        modeChoice_->SetStringSelection(MODE_STRING_BIN);
    }
    update();
}


/**
 * Sets the widget to hexadecimal mode.
 *
 * @exception NotAvailable If the MODE_HEXADECIMAL was not specified to the
 *                         constructor.
 */
void
NumberControl::setHexMode()
    throw (NotAvailable) {

    // Check that the hexadecimal mode is available.
    if ((style_ & MODE_HEXADECIMAL) == 0) {
        // Hexadecimal mode not available.
        string procName = "NubmerControl::setHexMode";
        throw NotAvailable(__FILE__, __LINE__, procName);
    }

    // Create hexadecimal input validator if necessary.
    if (hexValidator_ == NULL) {
        hexValidator_ =
            new wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST, &stringValue_);

#if wxCHECK_VERSION(2, 5, 4)
        wxArrayString includes;
        includes.Add(_T("0"));
        includes.Add(_T("1"));
        includes.Add(_T("2"));
        includes.Add(_T("3"));
        includes.Add(_T("4"));
        includes.Add(_T("5"));
        includes.Add(_T("6"));
        includes.Add(_T("7"));
        includes.Add(_T("8"));
        includes.Add(_T("9"));
        includes.Add(_T("a"));
        includes.Add(_T("b"));
        includes.Add(_T("c"));
        includes.Add(_T("d"));
        includes.Add(_T("e"));
        includes.Add(_T("f"));
        includes.Add(_T("A"));
        includes.Add(_T("B"));
        includes.Add(_T("C"));
        includes.Add(_T("D"));
        includes.Add(_T("E"));
        includes.Add(_T("F"));
        hexValidator_->SetIncludes(includes);
#else
        hexValidator_->SetIncludeList(
            wxStringList(_T("0"), _T("1"), _T("2"), _T("3"), _T("4"), _T("5"),
			 _T("6"), _T("7"), _T("8"), _T("9"),
                         _T("a"), _T("b"), _T("c"), _T("d"), _T("e"), _T("f"),
			 _T("A"), _T("B"), _T("C"), _T("D"),
                         _T("E"), _T("F"), NULL));
#endif
    }

    // Set the hexadecimal validator and update the widget.
    text_->SetValidator(*hexValidator_);
    mode_ = MODE_HEXADECIMAL;
    if (modeChoice_ != NULL &&
        modeChoice_->GetStringSelection() != MODE_STRING_HEX) {

        modeChoice_->SetStringSelection(MODE_STRING_HEX);
    }
    update();
}

/**
 * Sets the widget to unsigned int mode.
 *
 * @exception NotAvailable If the MODE_UNSIGNED style was not specified to the
 *                         constructor.
 */
void
NumberControl::setUnsignedMode()
    throw (NotAvailable) {

    // Check that the unsigned mode is available.
    if ((style_ & MODE_UNSIGNED) == 0) {
        // Unsigned mode not available.
        string procName = "NubmerControl::setUnsignedMode";
        throw NotAvailable(__FILE__, __LINE__, procName);
    }

    // Create decimal input validator if necessary.
    if (unsignedValidator_ == NULL) {
        unsignedValidator_ =
            new wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST, &stringValue_);

#if wxCHECK_VERSION(2, 5, 4)
        wxArrayString includes;
        includes.Add(_T("0"));
        includes.Add(_T("1"));
        includes.Add(_T("2"));
        includes.Add(_T("3"));
        includes.Add(_T("4"));
        includes.Add(_T("5"));
        includes.Add(_T("6"));
        includes.Add(_T("7"));
        includes.Add(_T("8"));
        includes.Add(_T("9"));
        unsignedValidator_->SetIncludes(includes);
#else
        unsignedValidator_->SetIncludeList(
            wxStringList(_T("0"), _T("1"), _T("2"), _T("3"), _T("4"), _T("5"),
			 _T("6"), _T("7"), _T("8"), _T("9"), NULL));
#endif
    }

    // Set the decimal validator and update the widget.
    text_->SetValidator(*unsignedValidator_);
    mode_ = MODE_UNSIGNED;
    if (modeChoice_ != NULL &&
        modeChoice_->GetStringSelection() != MODE_STRING_UNSIGNED) {

        modeChoice_->SetStringSelection(MODE_STRING_UNSIGNED);
    }
    update();
}


/**
 * Sets the widget to float mode.
 *
 * @exception NotAvailable If the MODE_FLOAT style was not specified to the
 *                         constructor.
 */
void
NumberControl::setFloatMode()
    throw (NotAvailable) {

    // Check that the float mode is available.
    if ((style_ & MODE_FLOAT) == 0) {
        // Float mode not available.
        string procName = "NubmerControl::setFloatMode";
        throw NotAvailable(__FILE__, __LINE__, procName);
    }

    // Create float input validator if necessary.
    if (floatValidator_ == NULL) {
        floatValidator_ =
            new wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST, &stringValue_);

#if wxCHECK_VERSION(2, 5, 4)
        wxArrayString includes;
        includes.Add(_T("0"));
        includes.Add(_T("1"));
        includes.Add(_T("2"));
        includes.Add(_T("3"));
        includes.Add(_T("4"));
        includes.Add(_T("5"));
        includes.Add(_T("6"));
        includes.Add(_T("7"));
        includes.Add(_T("8"));
        includes.Add(_T("9"));
        includes.Add(_T("e"));
        includes.Add(_T("."));
        includes.Add(_T("-"));
        includes.Add(_T("+"));
        floatValidator_->SetIncludes(includes);
#else
        floatValidator_->SetIncludeList(
            wxStringList(_T("0"), _T("1"), _T("2"), _T("3"), _T("4"),
			 _T("5"), _T("6"), _T("7"), _T("8"), _T("9"),
                         _T("e"), _T("."), _T("-"), _T("+"), NULL));
#endif
    }

    // Set the float validator and update the widget.
    text_->SetValidator(*floatValidator_);
    mode_ = MODE_FLOAT;
    if (modeChoice_ != NULL &&
        modeChoice_->GetStringSelection() != MODE_STRING_FLOAT) {

        modeChoice_->SetStringSelection(MODE_STRING_FLOAT);
    }
    update();
}


/**
 * Sets the widget to double mode.
 *
 * @exception NotAvailable If the MODE_DOUBLE style was not specified to the
 *                         constructor.
 */
void
NumberControl::setDoubleMode()
    throw (NotAvailable) {

    // Check that the double mode is available.
    if ((style_ & MODE_DOUBLE) == 0) {
        // Double mode not available.
        string procName = "NubmerControl::setDoubleMode";
        throw NotAvailable(__FILE__, __LINE__, procName);
    }

    // Create float/double input validator if necessary.
    if (floatValidator_ == NULL) {
        floatValidator_ =
            new wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST, &stringValue_);

#if wxCHECK_VERSION(2, 5, 4)
        wxArrayString includes;
        includes.Add(_T("0"));
        includes.Add(_T("1"));
        includes.Add(_T("2"));
        includes.Add(_T("3"));
        includes.Add(_T("4"));
        includes.Add(_T("5"));
        includes.Add(_T("6"));
        includes.Add(_T("7"));
        includes.Add(_T("8"));
        includes.Add(_T("9"));
        includes.Add(_T("e"));
        includes.Add(_T("."));
        includes.Add(_T("-"));
        includes.Add(_T("+"));
        floatValidator_->SetIncludes(includes);
#else
        floatValidator_->SetIncludeList(
            wxStringList(_T("0"), _T("1"), _T("2"), _T("3"), _T("4"),
			 _T("5"), _T("6"), _T("7"), _T("8"), _T("9"),
                         _T("e"), _T("."), _T("-"), _T("+"), NULL));
#endif
    }

    // Set the float/double validator and update the widget.
    text_->SetValidator(*floatValidator_);
    mode_ = MODE_DOUBLE;
    if (modeChoice_ != NULL &&
        modeChoice_->GetStringSelection() != MODE_STRING_DOUBLE) {

        modeChoice_->SetStringSelection(MODE_STRING_DOUBLE);
    }
    update();
}


/**
 * Returns the widget mode style flag.
 *
 * @return Current mode of the widget.
 */
long
NumberControl::mode() const {
    return mode_;
}
