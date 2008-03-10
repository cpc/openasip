/**
 * @file NullImmediateUnit.cc
 *
 * Implementation of NullImmediateUnit class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: yellow
 */

#include "NullImmediateUnit.hh"

namespace TTAMachine {

NullImmediateUnit NullImmediateUnit::instance_;

/**
 * The constructor.
 */
NullImmediateUnit::NullImmediateUnit() :
    ImmediateUnit("NULL", 1, 1, 1, 0, Machine::ZERO) {
}


/**
 * The destructor.
 */
NullImmediateUnit::~NullImmediateUnit() {
}


/**
 * Returns the only instance of NullImmediateUnit.
 *
 * @return The only NullImmediateUnit instance.
 */
NullImmediateUnit&
NullImmediateUnit::instance() {
    return instance_;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
Machine*
NullImmediateUnit::machine() const {
    abortWithError("machine()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception IllegalRegistration Never thrown.
 */
void
NullImmediateUnit::ensureRegistration(const Component&) const
    throw (IllegalRegistration) {

    abortWithError("ensureRegistration()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullImmediateUnit::isRegistered() const {
    abortWithError("isRegistered()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullImmediateUnit::hasPort(const std::string&) const {
    abortWithError("hasPort()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullImmediateUnit::portCount() const {
    abortWithError("portCount()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullImmediateUnit::maxReads() const {
    abortWithError("maxReads()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullImmediateUnit::maxWrites() const {
    abortWithError("maxWrites()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
RegisterFile::Type
NullImmediateUnit::type() const {
    abortWithError("type()");
    return RegisterFile::NORMAL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullImmediateUnit::isNormal() const {
    abortWithError("isNormal()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullImmediateUnit::isVolatile() const {
    abortWithError("isVolatile()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullImmediateUnit::isReserved() const {
    abortWithError("isReserved()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
std::string
NullImmediateUnit::name() const {
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
NullImmediateUnit::setName(const std::string&)
    throw (ComponentAlreadyExists, InvalidName) {

    abortWithError("setName()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
Machine::Extension
NullImmediateUnit::extensionMode() const {
    abortWithError("extensionMode()");
    return Machine::ZERO;
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullImmediateUnit::setMaxReads(int)
    throw (OutOfRange) {

    abortWithError("setMaxReads()");
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullImmediateUnit::setMaxWrites(int)
    throw (OutOfRange) {

    abortWithError("setMaxWrites()");
}


/**
 * Aborts the program with error message.
 */
void
NullImmediateUnit::setType(RegisterFile::Type) {
    abortWithError("setType()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullImmediateUnit::guardLatency() const {
    abortWithError("guardLatency()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullImmediateUnit::setGuardLatency(int)
    throw (OutOfRange) {

    abortWithError("setGuardLatency()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullImmediateUnit::latency() const {
    abortWithError("latency()");
    return 0;
}


/**
 * Aborts the program with error message.
 */
void
NullImmediateUnit::setExtensionMode(Machine::Extension) {
    abortWithError("setExtensionMode()");
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullImmediateUnit::setLatency(int)
    throw (OutOfRange) {

    abortWithError("setLatency()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullImmediateUnit::numberOfRegisters() const {
    abortWithError("numberOfRegisters()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullImmediateUnit::width() const {
    abortWithError("width()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullImmediateUnit::setNumberOfRegisters(int)
    throw (OutOfRange) {

    abortWithError("setNumberOfRegisters()");
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullImmediateUnit::setWidth(int)
    throw (OutOfRange) {

    abortWithError("setWidth()");
}


/**
 * Aborts the program with error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullImmediateUnit::setMachine(Machine&)
    throw (ComponentAlreadyExists) {

    abortWithError("setMachine()");
}


/**
 * Aborts the program with error message.
 */
void
NullImmediateUnit::unsetMachine() {
    abortWithError("unsetMachine()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
ObjectState*
NullImmediateUnit::saveState() const {
    abortWithError("saveState()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception ObjectStateLoadingException Never thrown.
 */
void
NullImmediateUnit::loadState(const ObjectState*)
    throw (ObjectStateLoadingException) {

    abortWithError("loadState()");
}

}
