/**
 * @file RemoteMemory.hh
 * 
 * Declaration of RemoteMemory class.
 * @author Kalle Raiskila, 2013
 */

#ifndef REMOTE_MEMORY_H
#define REMOTE_MEMORY_H

#include "Memory.hh"
#include "RemoteController.hh"
#include "AddressSpace.hh"

using TTAMachine::AddressSpace;

/**
 * RemoteMemory is a proxy class for a physical memory.
 * 
 * This class adapts a physical memory on a FPGA or ASIC, 
 * as seen through a debug interface, to the main TCE system.
 * The RemoteMemory is used only in the Simulator parts of TCE.
 * All accesses to the physical memories RemoteMemory objects do
 * go through the RemoteController that implements the debugging
 * interface.
 */
class RemoteMemory : public Memory {
	public:
	// TODO: hoist AddressSpace-based constructor to parent class?
	RemoteMemory( const AddressSpace &space )
	:Memory( space.start(), space.end(), space.width()), 
	 controller_(NULL), addressspace_(space) {}

	/** Set SimulationController for memory.
	 * The RemoteMemory accesses physical memories via this controller object.
	 */
	void setController(RemoteController* con) {
		controller_=con;
	}

	// overload the pure viruals of Memory
	virtual void write(Word address, MAU data);
	virtual Memory::MAU read(Word address);
	
	private:
	RemoteController* controller_;
	const AddressSpace& addressspace_;

};

#endif 
