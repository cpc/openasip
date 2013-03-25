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
    PluginTools(bool lazyResolution = true, bool local = false);
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

    /// True if all undefined symbols should be resolved only when needed.
    bool lazyResolution_;
    /// True if the symbols defined in the loaded library should be made
    /// available for symbol resolution of subsequently loaded libraries.
    bool localResolution_;

};

#include "PluginTools.icc"

#endif
