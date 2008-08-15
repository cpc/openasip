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
 * @file CostFunctionPlugin.hh
 *
 * Declaration of CostFunctionPlugin class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_COST_FUNCTION_PLUGIN_HH
#define TTA_COST_FUNCTION_PLUGIN_HH

#include <string>

namespace HDB {

/**
 * Class that contains data stored in HDB of a cost function plugin.
 */
class CostFunctionPlugin {
public:
    /// all supported cost function plugin types
    enum CostFunctionPluginType {
        COST_FU,     ///< function unit cost estimator
        COST_RF,     ///< register file cost estimator
        COST_DECOMP, ///< decompressor cost estimator
        COST_ICDEC   ///< interconnection network & decoder cost estimator
    };

    CostFunctionPlugin(
        int id,
        const std::string& description,
        const std::string& name,
        const std::string& pluginFilePath,
        CostFunctionPluginType type);
    virtual ~CostFunctionPlugin();

    int id() const;
    std::string description() const;
    std::string name() const;
    std::string pluginFilePath() const;
    CostFunctionPluginType type() const;

private:
    /// id of the plugin in HDB
    int id_;
    /// description of the plugin HDB
    std::string description_;
    /// (the class) name of the plugin
    std::string name_;
    /// the path in the file system of the plugin
    std::string pluginFilePath_;
    /// the type of the plugin
    CostFunctionPluginType type_;
};

}

#endif
