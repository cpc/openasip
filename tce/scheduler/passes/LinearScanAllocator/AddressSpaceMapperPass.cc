/** 
 * @file AddressSpaceMapperPass.cc
 *
 * Implementation of AddressSpaceMapperPass class
 *
 * scheduler pass for address space mapping.
 *
 * @author Heikki Kultala 2006 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#include "AddressSpaceMapper.hh"

#include "AddressSpaceMapperPass.hh"

using namespace TTAProgram;
using namespace TTAMachine;

SCHEDULER_PASS(AddressSpaceMapperPass)

AddressSpaceMapperPass::AddressSpaceMapperPass() {}

AddressSpaceMapperPass::~AddressSpaceMapperPass() {}

void AddressSpaceMapperPass::start() 
    throw (Exception) {
    AddressSpaceMapper::mapAddressSpace(*program_,*target_);
}

/**
 * A short description of the module, usually the module name,
 * in this case "AddressSpaceMapperPass".
 *
 * @return The description as a string.
 */   
std::string
AddressSpaceMapperPass::shortDescription() const {
    return "Startable: AddressSpaceMapperPass";
}

/**
 * Optional longer description of the Module.
 *
 * This description can include usage instructions, details of choice of
 * helper modules, etc.
 *
 * @return The description as a string.
 */
std::string
AddressSpaceMapperPass::longDescription() const {
    std::string answer = "Startable: AddressSpaceMapperPass";
    return answer;
}
