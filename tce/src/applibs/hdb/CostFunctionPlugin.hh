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
 * @file CostFunctionPlugin.hh
 *
 * Declaration of CostFunctionPlugin class.
 *
 * @author Pekka Jääskeläinen 2005 (pekka.jaaskelainen-no.spam-tut.fi)
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
