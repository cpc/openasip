/**
 * @file CompiledSimCompiler.cc
 *
 * Definition of CompiledSimCompiler class.
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <ctime>
#include <cstdlib>

#include "CompiledSimCompiler.hh"
#include "Conversion.hh"
#include "config.h"
#include "Application.hh"

using std::string;
using std::endl;
using std::vector;
using std::time_t;


/**
 * The constructor
 */
CompiledSimCompiler::CompiledSimCompiler() {
    
    // Get number of threads and the default compiler
    threadCount_ = 3;
    const char* USER_THREAD_COUNT = std::getenv("TTASIM_COMPILER_THREADS");
    if (USER_THREAD_COUNT != NULL) {
        threadCount_ = Conversion::toInt(string(USER_THREAD_COUNT));
    }
    
    compiler_ = "gcc";
    const char* USER_COMPILER = std::getenv("TTASIM_COMPILER");
    if (USER_COMPILER != NULL) {
        compiler_ = string(USER_COMPILER);
    }    
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
 * @return Return value given by system(). 0 on success. !=0 on failure
 */
int
CompiledSimCompiler::compileDirectory(
    const string& dirName,
    const string& flags) const {

    string command = 
        "make -sC " + dirName + " CC=\"" + compiler_ + "\" opt_flags=\"" + 
        flags + "\" -j" + Conversion::toString(threadCount_);

    Application::logStream()
        << "Compiling the simulation engine with command " << command << endl;

    time_t startTime = std::time(NULL);
    int retval = system(command.c_str());
    time_t endTime = std::time(NULL);

    time_t elapsed = endTime - startTime;

    Application::logStream()
        << "Compiling the simulation engine with opt. switches '" << flags 
        << "' took " << elapsed / 60 << "m " << (elapsed % 60) << "s " 
        << endl;

    return retval;
}

/**
 * Compiles a single C++ file using the set flags, outputs a .so file
 * 
 * Used for generating .so files needed for dynamic compiled simulation
 * 
 * @param filePath path to the file to be compiled
 * @param flags flags to be used for compiling
 */
int
CompiledSimCompiler::compileFile(
    const std::string& filePath,
    const std::string& flags) const {

    string command = 
        compiler_ + " " + "`tce-config --includes`" + " -shared -fpic "
            + "-fno-working-directory -fno-enforce-eh-specs -fno-rtti "
            + "-fno-threadsafe-statics -fno-access-control"
            + flags + " " + filePath;
    
    return system(command.c_str());
}
