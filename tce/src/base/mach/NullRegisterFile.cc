/**
 * @file NullRegisterFile.cc
 *
 * Implementation of NullRegisterFile class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: yellow
 */

#include "NullRegisterFile.hh"

namespace TTAMachine {

NullRegisterFile NullRegisterFile::instance_;

/**
 * The constructor.
 */
NullRegisterFile::NullRegisterFile() :
    RegisterFile("NULL", 1, 1, 1, 1, 0, RegisterFile::NORMAL) {
}


/**
 * The destructor.
 */
NullRegisterFile::~NullRegisterFile() {
}


/**
 * Returns the only instance of NullRegisterFile.
 */
NullRegisterFile&
NullRegisterFile::instance() {
    return instance_;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
Machine*
NullRegisterFile::machine() const {
    abortWithError("machine()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception IllegalRegistration Never thrown.
 */
void
NullRegisterFile::ensureRegistration(const Component&) const
    throw (IllegalRegistration) {

    abortWithError("ensureRegistration()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullRegisterFile::isRegistered() const {
    abortWithError("isRegistered()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullRegisterFile::hasPort(const std::string&) const {
    abortWithError("hasPort()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullRegisterFile::portCount() const {
    abortWithError("portCount()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullRegisterFile::setMachine(Machine&)
    throw (ComponentAlreadyExists) {

    abortWithError("setMachine");
}


/**
 * Aborts the program with error message.
 */
void
NullRegisterFile::unsetMachine() {
    abortWithError("unsetMachine()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullRegisterFile::numberOfRegisters() const {
    abortWithError("numberOfRegisters()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullRegisterFile::width() const {
    abortWithError("width()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullRegisterFile::setNumberOfRegisters(int)
    throw (OutOfRange) {

    abortWithError("setNumberOfRegisters()");
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullRegisterFile::setWidth(int)
    throw (OutOfRange) {

    abortWithError("setWidth()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullRegisterFile::maxReads() const {
    abortWithError("maxReads()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullRegisterFile::maxWrites() const {
    abortWithError("maxWrites()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
RegisterFile::Type
NullRegisterFile::type() const {
    abortWithError("type()");
    return RegisterFile::NORMAL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullRegisterFile::isNormal() const {
    abortWithError("isNormal()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullRegisterFile::isVolatile() const {
    abortWithError("isVolatile()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullRegisterFile::isReserved() const {
    abortWithError("isReserved()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
std::string
NullRegisterFile::name() const {
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
NullRegisterFile::setName(const std::string&)
    throw (ComponentAlreadyExists, InvalidName) {

    abortWithError("setName()");
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullRegisterFile::setMaxReads(int)
    throw (OutOfRange) {

    abortWithError("setMaxReads()");
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullRegisterFile::setMaxWrites(int)
    throw (OutOfRange) {

    abortWithError("setMaxWrites()");
}


/**
 * Aborts the program with error message.
 */
void
NullRegisterFile::setType(RegisterFile::Type) {
    abortWithError("setType()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullRegisterFile::guardLatency() const {
    abortWithError("guardLatency()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullRegisterFile::setGuardLatency(int)
    throw (OutOfRange) {

    abortWithError("setGuardLatency()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
ObjectState*
NullRegisterFile::saveState() const {
    abortWithError("saveState()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception ObjectStateLoadingException Never thrown.
 */
void
NullRegisterFile::loadState(const ObjectState*)
    throw (ObjectStateLoadingException) {

    abortWithError("loadState()");
}
}
