/**
 * @file HDBRegistry.cc
 *
 * Implementation of HDBRegistry class.
 *
 * @author Pekka Jääskeläinen 2005 (pekka.jaaskelainen@tut.fi)
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
HDBRegistry::hdb(const std::string fileName)
    throw (Exception) {

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
HDBRegistry::hdb(unsigned int index) 
    throw (OutOfRange) {

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
HDBRegistry::hdbPath(unsigned index) 
    throw (OutOfRange) {

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
HDBRegistry::hdbErrorMessage(unsigned int index)
    throw (OutOfRange) {

    if (index > (errorMessages_.size() - 1)) {
        throw OutOfRange(__FILE__, __LINE__,
                         "HDBRegistry::hdbErrorMessage(unsigned int)");
    }
    return errorMessages_[index];
}    

}
