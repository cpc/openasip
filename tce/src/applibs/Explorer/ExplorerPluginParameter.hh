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
