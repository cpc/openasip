/**
 * @file NullAddressSpace.cc
 *
 * Implementation of NullAddressSpace class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: yellow
 */

#include "NullAddressSpace.hh"

namespace TTAMachine {

Machine NullAddressSpace::machine_;
NullAddressSpace NullAddressSpace::instance_;

/**
 * The constructor.
 */
NullAddressSpace::NullAddressSpace() :
    AddressSpace("NULL", 1, 0, 1, machine_) {
}


/**
 * The destructor.
 */
NullAddressSpace::~NullAddressSpace() {
}


/**
 * Returns the only instance of NullAddressSpace.
 *
 * @return The only instance of NullAddressSpace.
 */
NullAddressSpace&
NullAddressSpace::instance() {
    return instance_;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullAddressSpace::width() const {
    abortWithError("width()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
unsigned int
NullAddressSpace::start() const {
    abortWithError("start()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
unsigned int
NullAddressSpace::end() const {
    abortWithError("end()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
std::string
NullAddressSpace::name() const {
    abortWithError("name()");
    return "";
}


/**
 * Aborts the program with error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 * @exception InvalidName Never thrown.
 */
void
NullAddressSpace::setName(const std::string&)
    throw (ComponentAlreadyExists, InvalidName) {

    abortWithError("setName()");
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullAddressSpace::setWidth(int)
    throw (OutOfRange) {

    abortWithError("setWidth()");
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullAddressSpace::setAddressBounds(unsigned int, unsigned int)
    throw (OutOfRange) {

    abortWithError("setAddressBounds()");
}


/**
 * Aborts the program with error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullAddressSpace::setMachine(Machine&)
    throw (ComponentAlreadyExists) {

    abortWithError("setMachine()");
}

/**
 * Aborts the program with error message.
 */
void
NullAddressSpace::unsetMachine() {
    abortWithError("unsetMachine()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
Machine*
NullAddressSpace::machine() const {
    abortWithError("machine()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception IllegalRegistration Never thrown.
 */
void
NullAddressSpace::ensureRegistration(const Component&) const
    throw (IllegalRegistration) {

    abortWithError("ensureRegistration()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullAddressSpace::isRegistered() const {
    abortWithError("isRegistered()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
ObjectState*
NullAddressSpace::saveState() const {
    abortWithError("saveState()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception ObjectStateLoadingException Never thrown.
 */
void
NullAddressSpace::loadState(const ObjectState*)
    throw (ObjectStateLoadingException) {

    abortWithError("loadState()");
}

}
