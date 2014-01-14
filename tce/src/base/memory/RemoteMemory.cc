/** 
 * @file RemoteMemory.cc
 * 
 * Implementation of RemoteMemory class.
 * @author Kalle Raiskila
 */
#include <assert.h>
#include "RemoteMemory.hh"

typedef MinimumAddressableUnit MAU;

void RemoteMemory::write(Word address, MAU data)
{
	assert(controller_);
	controller_->writeMem( address, data, addressspace_);
}

Memory::MAU RemoteMemory::read(Word address)
{
	assert(controller_);
	return controller_->readMem( address, addressspace_ );
}
