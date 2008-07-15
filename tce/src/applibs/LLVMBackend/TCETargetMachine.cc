/**
 * @file TCETargeMachine.cpp
 *
 * Declaration of TCETargetMachine class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
 */

#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Target/TargetMachineRegistry.h"
#include "llvm/Target/MRegisterInfo.h"

#include "TCETargetMachine.hh"
#include "TCETargetAsmInfo.hh"
#include "LLVMPOMBuilder.hh"
#include "PluginTools.hh"
#include "FileSystem.hh"
#include "ADFSerializer.hh"

#include <iostream>

using namespace llvm;

//namespace {
//    RegisterTarget<TCETargetMachine>  X("tce", "TCE");
//}

const TargetAsmInfo*
TCETargetMachine::createTargetAsmInfo() const {
    return new TCETargetAsmInfo(*this);
}

//
// Data layout:
//--------------
// E-p:32:32:32-i1:8:8-i8:8:8-i32:32:32-i64:32:64-f32:32:32-f64:32:64"
// E = Big endian data
// -p:32:32:32 = Pointer size 32 bits, api & preferred alignment 32 bits.
// -i8:8:8 = 8bit integer api & preferred alignment 8 bits.
// etc.
// 
//  Frame info:
// -------------
// Grows down, alignment 4 bytes.
//
TCETargetMachine::TCETargetMachine(
    const Module& m, const std::string& fs, TCETargetMachinePlugin& plugin)
    : dataLayout_(
        "E-p:32:32:32"
	"-a0:32:32"
        "-i1:8:8"
        "-i8:8:8"
        "-i32:32:32"
        "-i64:32:64"
        "-f32:32:32"
        "-f64:32:64"),
      subtarget_(m, fs),
      frameInfo_(TargetFrameInfo::StackGrowsDown, 4, -4),
      plugin_(plugin), pluginTool_(NULL) {

}

/**
 * The Destructor.
 */
TCETargetMachine::~TCETargetMachine() {
    if (pluginTool_ != NULL) {
        delete pluginTool_;
        pluginTool_ = NULL;
    }
}

/**
 * Checks how well target triple from llvm-gcc matches with this architecture.
 */
unsigned 
TCETargetMachine::getModuleMatchQuality(const Module &M) {
    std::string TT = M.getTargetTriple();
    if (TT.size() >= 4 && std::string(TT.begin(), TT.begin()+4) == "tce-") {
        return 20;
    }

    return 0;
}

/**
 * Creates an instruction selector instance.
 */
bool
TCETargetMachine::addInstSelector(FunctionPassManager& pm, bool /* fast */) {
    FunctionPass* isel = plugin_.createISelPass(this);
    pm.add(isel);
    return false;
}


/**
 * Creates a code printer instance.
 *
 * @param pm Pass manager where the code printer pass is added.
 * @param fast Unused.
 * @param out Output stream for the code printer.
 */
bool
TCETargetMachine::addAssemblyEmitter(
    FunctionPassManager& pm, bool /* fast */, std::ostream& out) {

    // Output assembly language.
    pm.add(plugin_.createAsmPrinterPass(out, this));
    return false;
}

TTAMachine::Machine*
TCETargetMachine::createMachine() {
    ADFSerializer serializer;
    serializer.setSourceString(*plugin_.adfXML());
    return serializer.readMachine();
}


/**
 * Loads the plugin file specified with the -tce-plugin option.

void
TCETargetMachine::loadPlugin() {

    std::string pluginFileName = subtarget_.pluginFileName();
    if (pluginFileName == "") {
        std::cerr << "No target machine plugin name specified." << std::endl;
        assert(false);
    }
    if (!FileSystem::fileExists(pluginFileName) ||
        !FileSystem::fileIsReadable(pluginFileName)) {

        std::cerr << "Plugin file '" << pluginFileName
                  << "' does not exist or is not readable."
                  << std::endl;

        assert(false);
    }

    std::string pluginPath =
        FileSystem::directoryOfPath(subtarget_.pluginFileName());

    std::string pluginFile =
        FileSystem::fileOfPath(subtarget_.pluginFileName());

    pluginTool_ = new PluginTools();

    try {
        pluginTool_->addSearchPath(pluginPath);
        pluginTool_->registerModule(pluginFile);
        TCETargetMachinePlugin* (*creator)();
        pluginTool_->importSymbol(
            "create_tce_backend_plugin", creator, pluginFile);
        
        plugin_ = creator();
    } catch (Exception& e) {
        std::cerr << "ERROR loading '" << pluginPath << "/"
                  << pluginFile << "': "
                  << e.errorMessage() << std::endl;

        assert(false);
    }


#if 0
    // DEBUG: Print register statistics.
    std::cerr << "REGISTER INFO:" << std::endl;
    std::cerr << "--------------" << std::endl;
    const MRegisterInfo* ri = plugin_.getRegisterInfo();
    assert(ri != NULL);
    for (unsigned i = 1; i <= ri->getNumRegClasses(); i++) {
        const TargetRegisterClass* rc = ri->getRegClass(i);
        assert(rc != NULL);
        std::cerr << "sz: " << rc->getSize()
                  << " align: " << rc->getAlignment()
                  << " regs: " << rc->getNumRegs() << std::endl;
    }
    std::cerr << "--------------" << std::endl;
#endif

}
*/
