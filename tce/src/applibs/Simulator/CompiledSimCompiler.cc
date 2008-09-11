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
 * @file CompiledSimCompiler.cc
 *
 * Definition of CompiledSimCompiler class.
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <ctime>
#include <cstdlib>

#include "CompiledSimCompiler.hh"
#include "Conversion.hh"
#include "config.h"
#include "Application.hh"
#include "Environment.hh"
#include "FileSystem.hh"

using std::string;
using std::endl;
using std::vector;
using std::time_t;


// Initialize statics

// the most important one is "fno-working-directory" which is used because 
// ccache doesn't like changing directory paths. Rest is just minor tweaks.
const char* CompiledSimCompiler::COMPILED_SIM_CPP_FLAGS = 
    " -fno-working-directory "
    "-fno-enforce-eh-specs "
    "-fno-rtti "
    "-fno-threadsafe-statics "
    "-fvisibility=hidden "
    "-fvisibility-inlines-hidden ";

// this was not recognized on Ubuntu 6.06's gcc:
//    "-fno-stack-protector "

// Flags when compiling shared libraries
const char* CompiledSimCompiler::COMPILED_SIM_SO_FLAGS = " -shared -fpic ";

/**
 * The constructor
 */
CompiledSimCompiler::CompiledSimCompiler() {
    
    // Get number of threads
    threadCount_ = 3;
    const char* USER_THREAD_COUNT = std::getenv("TTASIM_COMPILER_THREADS");
    if (USER_THREAD_COUNT != NULL) {
        threadCount_ = Conversion::toInt(string(USER_THREAD_COUNT));
    }
    
    // Get compiler
    compiler_ = "gcc";
    const char* USER_COMPILER = std::getenv("TTASIM_COMPILER");
    if (USER_COMPILER != NULL) {
        compiler_ = string(USER_COMPILER);
    }
    
    // Get global compile flags
    globalCompileFlags_ = " -O0 ";
    const char* fl = std::getenv("TTASIM_COMPILER_FLAGS");
    if (fl != NULL)
        globalCompileFlags_ = std::string(fl);
}

/**
 * The destructor
 */
CompiledSimCompiler::~CompiledSimCompiler() {
}
    
/**
 * Compiles a directory with given flags using a pre-generated makefile
 * 
 * In case environment variable TTASIM_COMPILER is set, it is used
 * to compile the simulation code, otherwise 'gcc' is used. The
 * count of compiler threads is read from TTASIM_COMPILER_THREADS,
 * and defaults to 3.
 *
 * @param dirName a source directory containing the .cpp files and the Makefile
 * @param flags additional compile flags given by the user. for instance, "-O3"
 * @param verbose Print information of the compilation progress.
 * @return Return value given by system(). 0 on success. !=0 on failure
 */
int
CompiledSimCompiler::compileDirectory(
    const string& dirName,
    const string& flags,
    bool verbose) const {

    string command = 
        "make -sC " + dirName + " CC=\"" + compiler_ + "\" opt_flags=\"" +
        globalCompileFlags_ + " " + flags + " \" -j" +
        Conversion::toString(threadCount_);

    if (verbose) {
        Application::logStream()
            << "Compiling the simulation engine with command " 
            << command << endl;
    }

    time_t startTime = std::time(NULL);
    int retval = system(command.c_str());
    time_t endTime = std::time(NULL);

    time_t elapsed = endTime - startTime;

    if (verbose) {
        Application::logStream()
            << "Compiling the simulation engine with opt. switches '"
            << globalCompileFlags_ << " " << flags 
            << "' took " << elapsed / 60 << "m " << (elapsed % 60) << "s " 
            << endl;
    }

    return retval;
}

/**
 * Compiles a single C++ file using the set flags
 * 
 * @param path Path to the file
 * @param flags custom flags to be used for compiling
 * @param outputExtension extension to append to the filename. default is ".o"
 * @param verbose Print information of the compilation progress.
 * @return Return value given by system() call. 0 on success. !=0 on failure
 */
int
CompiledSimCompiler::compileFile(
    const std::string& path,
    const string& flags,
    const string& outputExtension,
    bool verbose) const {

    std::string directory = FileSystem::directoryOfPath(path);
    std::string fileNameBody = FileSystem::fileNameBody(path);
    std::string fileName = FileSystem::fileOfPath(path);   
    std::string DS = FileSystem::DIRECTORY_SEPARATOR;
    
    // Get includes
    vector<string> includePaths = Environment::includeDirPaths();
    string includes;
    for (vector<string>::const_iterator it = includePaths.begin(); 
        it != includePaths.end(); ++it) {
        includes += "-I" + *it + " ";
    }

    if (verbose) {
        Application::logStream() << "Compiling simulation file " 
            << path << endl;
    }
    
    string command = compiler_ + " " + includes + COMPILED_SIM_CPP_FLAGS
        + globalCompileFlags_ + " " + flags + " " 
        + path + " -o " + directory + DS + fileNameBody + outputExtension;
    
    return system(command.c_str());
}

/**
 * Compiles a single C++ file to a shared library (.so)
 * 
 * Used for generating .so files in dynamic compiled simulation
 * 
 * @param path Path to the file
 * @param flags custom flags to be used for compiling
 * @param verbose Print information of the compilation progress.
 * @return Return value given by system() call. 0 on success. !=0 on failure
 */
int
CompiledSimCompiler::compileToSO(
    const std::string& path,
    const string& flags,
    bool verbose) const {

    return compileFile(path, COMPILED_SIM_SO_FLAGS + flags, ".so", verbose);
}

