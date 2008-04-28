/**
 * @file UnitImplementationLocation.cc
 *
 * Implementation of UnitImplementationLocation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <string>
#include <vector>

#include "UnitImplementationLocation.hh"
#include "MachineImplementation.hh"
#include "FileSystem.hh"
#include "ObjectState.hh"

using std::string;
using std::vector;

namespace IDF {

const string UnitImplementationLocation::OSNAME_UNIT_IMPLEMENTATION = 
    "unit_implementation";
const string UnitImplementationLocation::OSKEY_ID = "id";
const string UnitImplementationLocation::OSKEY_UNIT_NAME = "unit_name";
const string UnitImplementationLocation::OSKEY_HDB_FILE = "hdb_file";


/**
 * The constructor.
 *
 * @param hdbFile The database that contains the implementation.
 * @param id ID of the RF or FU entry in the database.
 * @param unitName Name of the corresponding unit in ADF.
 */
UnitImplementationLocation::UnitImplementationLocation(
    const std::string& hdbFile,
    int id,
    const std::string& unitName) :
    hdbFile_(hdbFile), id_(id), unitName_(unitName), parent_(NULL) {
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
UnitImplementationLocation::UnitImplementationLocation(
    const ObjectState* state)
    throw (ObjectStateLoadingException) :
    hdbFile_(""), id_(0), unitName_(""), parent_(NULL) {

    /// this is used by NullFUImplementationLocation
    if (state == NULL)
        return;
    loadState(state);
}


/**
 * The destructor.
 */
UnitImplementationLocation::~UnitImplementationLocation() {
}


/**
 * Returns the absolute path to the HDB file.
 *
 * @return Absolute path to the HDB file.
 * @exception FileNotFound If the HDB file is not found in the search paths.
 */
std::string
UnitImplementationLocation::hdbFile() const 
    throw (FileNotFound) {

    assert(parent_ != NULL);
    vector<string> paths = Environment::hdbPaths();
    paths.insert(
        paths.begin(), FileSystem::directoryOfPath(parent_->sourceIDF()));
    string expandedPath = FileSystem::expandTilde(hdbFile_);
    return FileSystem::findFileInSearchPaths(paths, expandedPath);
}


/**
 * Returns the entry ID in HDB.
 *
 * @return The entry ID.
 */
int
UnitImplementationLocation::id() const {
    return id_;
}


/**
 * Returns the name of the unit in ADF.
 *
 * @return The name of the unit.
 */
std::string
UnitImplementationLocation::unitName() const {
    return unitName_;
}


/**
 * Sets the parent of the object.
 *
 * @param parent The parent MachineImplementation instance.
 * @exception InvalidData If the parent is already set.
 */
void
UnitImplementationLocation::setParent(MachineImplementation& parent)
    throw (InvalidData) {
    
    if (parent_ != NULL) {
        throw InvalidData(__FILE__, __LINE__, __func__);
    }

    parent_ = &parent;
}


/**
 * Loads the state of the object from the given ObjectState instance.
 *
 * @exception ObjectStateLoadingException If the given ObjectState instance.
 */
void
UnitImplementationLocation::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    const string procName = "UnitImplementationLocation::loadState";

    if (state->name() != OSNAME_UNIT_IMPLEMENTATION) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    try {
        hdbFile_ = state->stringAttribute(OSKEY_HDB_FILE);
        id_ = state->intAttribute(OSKEY_ID);
        unitName_ = state->stringAttribute(OSKEY_UNIT_NAME);
    } catch (const Exception& exception) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }
}


/**
 * Saves the state of the object to an ObjectState instance.
 *
 * @return The newly created ObjectState instance.
 */
ObjectState*
UnitImplementationLocation::saveState() const {
    ObjectState* state = new ObjectState(OSNAME_UNIT_IMPLEMENTATION);
    state->setAttribute(OSKEY_HDB_FILE, hdbFile_);
    state->setAttribute(OSKEY_ID, id());
    state->setAttribute(OSKEY_UNIT_NAME, unitName());
    return state;
}

/**
 * Sets the absolute path to the HDB file.
 *
 * @param file Absolute path to the HDB file.
 */
void
UnitImplementationLocation::setHDBFile(std::string file) {
    hdbFile_ = file;
}


/**
 * Sets the entry ID in HDB.
 */
void
UnitImplementationLocation::setID(int id) {
    id_ = id;
}

}
