/**
 * @file TCETargetMachinePlugin.cc
 *
 * Implementation of TCETargetMachinePlugin class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <iostream>
#include "TCETargetMachinePlugin.hh"
#include "TCEPlugin.hh"
#include "TCEInstrInfo.hh"
#include "TCEAsmPrinter.hh"
#include "TCETargetMachine.hh"

using namespace llvm;

using std::string;

namespace llvm {
class GeneratedTCEPlugin : public TCETargetMachinePlugin {
public:
    GeneratedTCEPlugin();
    virtual ~GeneratedTCEPlugin();
    virtual const TargetInstrInfo* getInstrInfo() const;
    virtual const MRegisterInfo* getRegisterInfo() const;
    virtual FunctionPass* createISelPass(TCETargetMachine* tm);
    virtual FunctionPass* createAsmPrinterPass(
        std::ostream& o, TCETargetMachine* tm);

    virtual unsigned spDRegNum() {
        return TCE::SP;
    }
   
    virtual const std::string* adfXML() {
        return &adfXML_;
    }

    virtual std::string rfName(unsigned dwarfRegNum);
    virtual unsigned registerIndex(unsigned dwarfRegNum);

    unsigned int raPortDRegNum();
    std::string dataASName();

private:
    static const std::string adfXML_;
};
}

#include "Backend.inc"

/**
 * The Cosntructor.
 */
GeneratedTCEPlugin::GeneratedTCEPlugin(): TCETargetMachinePlugin() {
   instrInfo_ = new TCEInstrInfo();
}


/**
 * The Destructor.
 */
GeneratedTCEPlugin::~GeneratedTCEPlugin() {
   delete instrInfo_;
}


const TargetInstrInfo*
GeneratedTCEPlugin::getInstrInfo() const {
    return instrInfo_;
}

const MRegisterInfo*
GeneratedTCEPlugin::getRegisterInfo() const {
    return &(dynamic_cast<TCEInstrInfo*>(instrInfo_))->getRegisterInfo();
}


FunctionPass*
GeneratedTCEPlugin::createISelPass(TCETargetMachine* tm) {

    return createTCEISelDag(*tm);
}

FunctionPass*
GeneratedTCEPlugin::createAsmPrinterPass(
    std::ostream& o, TCETargetMachine* tm) {

    return new TCEAsmPrinter(o, *tm, tm->getTargetAsmInfo());
}

extern "C" {                                
    TCETargetMachinePlugin*
    create_tce_backend_plugin() {
       TCETargetMachinePlugin* instance =
	   new GeneratedTCEPlugin();
       return instance;
    }
    void delete_tce_backend_plugin(
        TCETargetMachinePlugin* target) {
        delete target;
    }
}
