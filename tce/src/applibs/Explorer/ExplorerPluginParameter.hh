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
 * @file ExplorerPluginParameter.hh
 *
 * Declaration of ExplorerPluginParameter class.
 *
 * @author Esa Määttä 2008 (esa.maatta-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_EXPLORER_PLUGIN_PARAMETER_HH
#define TTA_EXPLORER_PLUGIN_PARAMETER_HH


#include <string>

#include "Exception.hh"
    
enum ExplorerPluginParameterType { INT, UINT, STRING, BOOL };

/**
 * Explorer plugin parameter class. Represents a parameter that an explorer
 * plugin can have.
 */
class ExplorerPluginParameter {
public:
    ExplorerPluginParameter(
        std::string name, 
        ExplorerPluginParameterType type,
        bool compulsory,
        std::string value);

    virtual ~ExplorerPluginParameter();

    std::string name() const;
    std::string value() const;
    void setValue(const std::string& value);

    ExplorerPluginParameterType type() const;
    std::string typeAsString() const;

    bool isCompulsory() const;
    bool isSet() const;

private:
    /// Parameter name
    std::string name_;
    /// Parameter type
    ExplorerPluginParameterType type_;
    /// Is parameter compulsory
    bool compulsory_;
    /// The value of the parameter as a string, empty if none set.
    std::string value_;
};

#endif
