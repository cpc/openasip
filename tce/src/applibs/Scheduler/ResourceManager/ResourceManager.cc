/**
 * @file ResourceManager.cc
 *
 * Implementation of ResourceManager class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "ResourceManager.hh"

/**
 * Constructor.
 *
 * @param machine Target machine to be managed by the resource manager.
 */
ResourceManager::ResourceManager(const TTAMachine::Machine& machine):
    machine_(&machine) {
}

/**
 * Destructor.
 */
ResourceManager::~ResourceManager(){
}

/**
 * Return the target machine managed by the resource manager.
 *
 * @return The target machine managed by the resource manager.
 */
const TTAMachine::Machine&
ResourceManager::machine() const {
    return *machine_;
}
