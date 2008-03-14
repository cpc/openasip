/**
 * @file TCESubtarget.cpp
 *
 * Implementation of TCESubtargetClass.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
 */

#include <iostream>
#include "llvm/Support/CommandLine.h"

#include "TCESubtarget.hh"

using namespace llvm;

// Add plugin file name option.
cl::opt<std::string>
BackendPluginFile(
    "tce-plugin-file",
    cl::value_desc("plugin file"),
    cl::desc("TCE target machine plugin file."),
    cl::NotHidden);

/**
 * The Constructor.
 */
TCESubtarget::TCESubtarget(const Module& /* m */, const std::string& /* fs */) :
    pluginFile_(BackendPluginFile) {

}

/**
 * Returns full path of the plugin file name supplied with the
 * -tce-plugin-file parameter.
 *
 * @return plugin file path
 */
std::string
TCESubtarget::pluginFileName() {
    return pluginFile_;
}
