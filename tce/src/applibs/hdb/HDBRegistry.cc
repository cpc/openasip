/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file HDBRegistry.cc
 *
 * Implementation of HDBRegistry class.
 *
 * @author Pekka Jääskeläinen 2005 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */
#include "HDBRegistry.hh"
#include "AssocTools.hh"
#include "CachedHDBManager.hh"
#include "Application.hh"
#include "Environment.hh"
#include "FileSystem.hh"

namespace HDB {

HDBRegistry* HDBRegistry::instance_ = NULL;

/**
 * Constructor.
 */ 
HDBRegistry::HDBRegistry() {
}

/**
 * Destructor.
 */
HDBRegistry::~HDBRegistry() {
    AssocTools::deleteAllValues(registry_);
}

/**
 * Creates and returns an instance of HDB registry.
 *
 * @return An instance of HDB registry.
 */
HDBRegistry&
HDBRegistry::instance() {

    if (instance_ == NULL) {
        instance_ = new HDBRegistry();
    }
    return *instance_;
}

/**
 * Returns the HDB at given path.
 *
 * In case the HDB is not found in registry, tries to open it.
 *
 * @param fileName File name of the HDB.
 * @return The HDB associated with the file name.
 * @exception Exception In case there was a problem while opening the HDB.
 */
CachedHDBManager&
HDBRegistry::hdb(const std::string fileName) {
    if (AssocTools::containsKey(
            registry_, FileSystem::absolutePathOf(fileName))) {
        return *registry_[FileSystem::absolutePathOf(fileName)];
    }
    CachedHDBManager* manager = &CachedHDBManager::instance(
        FileSystem::absolutePathOf(fileName));

    registry_[FileSystem::absolutePathOf(fileName)] = manager;
    return *manager;
}

/**
 * Adds HDBManager in to the registry.
 *
 * Takes responsibility of deleting the given HDBManager.
 *
 * @param hdb The HDBManager to be added in to the registry.
 */
void
HDBRegistry::addHDB(CachedHDBManager* hdbManager) {

    if (AssocTools::containsKey(
            registry_, FileSystem::absolutePathOf(hdbManager->fileName()))) {

        delete hdbManager;
        hdbManager = NULL;
        return;
    }
    registry_[FileSystem::absolutePathOf(hdbManager->fileName())] = hdbManager;
}

/**
 * Returns true if the registry contains a HDBManager of the given HDB file.
 *
 * @param hdbFile The HDB file managed by the HDBManager.
 * @return True if the registry contains a HDBManager of the given HDB file.
 */
bool
HDBRegistry::hasHDB(const std::string& hdbFile) {

    if (AssocTools::containsKey(
            registry_, FileSystem::absolutePathOf(hdbFile))) {
        return true;
    } else {
        return false;
    }
}

/**
 * Returns the total number of stored HDBs.
 *
 * @return Count of stored HDBs.
 */
int
HDBRegistry::hdbCount() {

    return registry_.size();
}

/*
 * Searches and loads all HDB:s found in Environment paths. 
 *
 * Stores erros caught in loading HDBs.
 */
void
HDBRegistry::loadFromSearchPaths() {

    // check every HDB path exists, and if doesn't, remove from registry
    removeDeadHDBPaths();

    const std::vector<std::string> hdbPaths = Environment::hdbPaths();
    std::vector<std::string>::const_iterator hdbIter = hdbPaths.begin();
    for(; hdbIter != hdbPaths.end(); hdbIter++) {
        if (!FileSystem::fileExists(*hdbIter) ||
            !FileSystem::fileIsDirectory(*hdbIter) ||
            !FileSystem::fileIsReadable(*hdbIter)) {
            // Directory doesn't exist or can't be read.
            continue;
        }
        std::vector<std::string> dir = 
            FileSystem::directoryContents(*hdbIter);
        std::vector<std::string>::const_iterator dirIter = dir.begin();
        for (; dirIter != dir.end(); dirIter++) {
            std::string file = (*dirIter);
            if (file.rfind(".hdb", file.size()) != std::string::npos &&
                file.rfind(".hdb", file.size()) == (file.size() - 4)) {
                if (!AssocTools::containsKey(registry_, file)) {
                    try {
                        CachedHDBManager* manager =
                            &CachedHDBManager::instance(
                                FileSystem::absolutePathOf(file));

                        registry_[FileSystem::absolutePathOf(file)] = manager;
                    } catch (IOException& e) {
                        std::string errorMessage = "Error in '" + file +
                            "': " + e.errorMessage() + ".";
                        errorMessages_.push_back(errorMessage);
                    }
                }
            }
        }
    }
}

/**
 * Returns the HDB from given index.
 *
 * @return The HDB from the given index.
 * @exception OutOfRange Is thrown if index is bigger than the HDB count.
 */
CachedHDBManager&
HDBRegistry::hdb(unsigned int index) {
    if (index > (registry_.size() - 1)) {
        throw OutOfRange(__FILE__, __LINE__,
                         "HDBRegistry::hdb(unsigned int)");
    }
    std::map<const std::string, CachedHDBManager*>::const_iterator 
        mapIterator = registry_.begin();
    for (unsigned int counter = 0; mapIterator != registry_.end();
         mapIterator++) {
        if (counter == index) {
            break;
        } else {
            counter++;
        }
    }
    return *(*mapIterator).second;
}

/**
 * Returns the full path of the HDB with given index.
 *
 * @return The HDB file path.
 * @exception OutOfRange Is thrown if index is out of range.
 */
std::string
HDBRegistry::hdbPath(unsigned index) {
    if (index > (registry_.size() - 1)) {
        throw OutOfRange(__FILE__, __LINE__,
                         "HDBRegistry::hdb(unsigned int)");
    }
    std::map<const std::string, CachedHDBManager*>::const_iterator iter = 
        registry_.begin();

    for (unsigned c = 0; iter != registry_.end(); iter++) {
        if (c == index) return (*iter).first;
        c++;
    }
    assert(false);
}

/**
 * Returns the number of error messages created during HDB loading.
 *
 * @return The number of error messages created during HDB loading.
 */
int 
HDBRegistry::hdbErrorCount() {

    return errorMessages_.size();
}

/**
 * Returns the error message from given index.
 *
 * @return The error message in given index.
 * @exception OutOfRange Is thrown if index is bigger than the HDB 
 * error count.
 */
std::string
HDBRegistry::hdbErrorMessage(unsigned int index) {
    if (index > (errorMessages_.size() - 1)) {
        throw OutOfRange(__FILE__, __LINE__,
                         "HDBRegistry::hdbErrorMessage(unsigned int)");
    }
    return errorMessages_[index];
}

/**
 * Removes nonexistent HDB files from registry.
 */
void
HDBRegistry::removeDeadHDBPaths() {
    
    std::map<const std::string, CachedHDBManager*>::iterator it;
    for (it = registry_.begin(); it != registry_.end(); ) {
        if (!FileSystem::fileExists(it->first)) {
            registry_.erase(it->first);
            ++it;
        } else {
            ++it;
        }
    }
}

}
