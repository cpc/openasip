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
 * @file PluginTools.hh
 *
 * Declaration of PluginTools class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note reviewed 19 May 2004 by ml, jn, ao, am
 * @note rating: green
 */

#ifndef TTA_PLUGIN_TOOLS_HH
#define TTA_PLUGIN_TOOLS_HH

#include <string>
#include <vector>
#include <map>

#include "Exception.hh"


/**
 * Class that handles accessing of dynamic modules for providing run-time
 * plug-in functionality.
 *
 * Class allows defining search paths, in which modules are searched. This
 * enables registering modules without absolut paths to modules. Symbols
 * (variables, functions) are loaded from modules and automatically cast
 * to correct types by means of the template mechanism.
 */
class PluginTools {
public:
    PluginTools(bool modeLazyGlobal = false);
    virtual ~PluginTools();

    void addSearchPath(const std::string& searchPath)
        throw (FileNotFound);
    void removeSearchPath(const std::string& searchPath);
    void clearSearchPaths();

    void registerModule(const std::string& module)
        throw (FileNotFound, DynamicLibraryException);
    void unregisterModule(const std::string& module)
        throw (FileNotFound, DynamicLibraryException, MultipleInstancesFound);
    void unregisterAllModules()
        throw (DynamicLibraryException);

    template <typename T>
    void importSymbol(
        const std::string& symbolName,
        T*& target,
        const std::string& module)
        throw (MultipleInstancesFound, FileNotFound, DynamicLibraryException,
               SymbolNotFound);

    template <typename T>
    void importSymbol(const std::string& symbolName, T*& target)
        throw (MultipleInstancesFound, FileNotFound, DynamicLibraryException,
               SymbolNotFound);

private:
    typedef std::vector<std::string>::iterator VecIter;
    typedef std::map<std::string, void*>::iterator MapIter;
    typedef std::map<std::string, void*>::value_type ValType;

    void* loadSym(
        const std::string& symbolName,
        const std::string& module = "")
        throw (MultipleInstancesFound, DynamicLibraryException, FileNotFound,
               SymbolNotFound);

    std::string findModule(const std::string& module)
        throw (MultipleInstancesFound, FileNotFound);

    /// Copying not allowed.
    PluginTools(const PluginTools&);
    /// Assignment not allowed.
    PluginTools& operator=(const PluginTools&);

    /// Search paths of dynamic modules.
    std::vector<std::string> searchPaths_;
    /// Map containing opened module handles.
    std::map<std::string, void*> modules_;

    /// True if the symbols defined in the loaded library should be made
    /// available for symbol resolution of subsequently loaded libraries,
    /// and if all undefined symbols should be resolved only when needed
    /// (use flags RTLD_GLOBAL and RTLD_LAZY).
    bool lazyResolution_;
};

#include "PluginTools.icc"

#endif
