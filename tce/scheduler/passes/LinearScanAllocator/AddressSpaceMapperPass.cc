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
