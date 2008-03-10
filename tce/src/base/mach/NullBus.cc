/**
 * @file NullBus.cc
 *
 * Implementation of NullBus class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: yellow
 */

#include "NullBus.hh"

namespace TTAMachine {

NullBus NullBus::instance_;

/**
 * The constructor.
 */
NullBus::NullBus() : Bus("NULL", 1, 1, Machine::ZERO) {
}


/**
 * The destructor.
 */
NullBus::~NullBus() {
}


/**
 * Returns the only instance of NullBus.
 *
 * @return The only instance of NullBus.
 */
NullBus&
NullBus::instance() {
    return instance_;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
std::string
NullBus::name() const {
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
NullBus::setName(const std::string&)
    throw (ComponentAlreadyExists, InvalidName) {

    abortWithError("setName()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 * @exception NotAvailable Never thrown.
 */
int
NullBus::position() const
    throw (NotAvailable) {

    abortWithError("position()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullBus::width() const {
    abortWithError("width()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullBus::immediateWidth() const {
    abortWithError("immediateWidth()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullBus::signExtends() const {
    abortWithError("signExtends()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullBus::zeroExtends() const {
    abortWithError("zeroExtends()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullBus::setWidth(int)
    throw (OutOfRange) {

    abortWithError("setWidth()");
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullBus::setImmediateWidth(int)
    throw (OutOfRange) {

    abortWithError("setImmediateWidth()");
}


/**
 * Aborts the program with error message.
 */
void
NullBus::setZeroExtends() {
    abortWithError("setZeroExtends()");
}


/**
 * Aborts the program with error message.
 */
void
NullBus::setSignExtends() {
    abortWithError("setSignExtends()");
}


/**
 * Aborts the program with error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullBus::addSegment(Segment&)
    throw (ComponentAlreadyExists) {

    abortWithError("addSegment()");
}


/**
 * Aborts the program with error message.
 */
void
NullBus::removeSegment(Segment&) {
    abortWithError("removeSegment()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullBus::hasSegment(const std::string&) const {
    abortWithError("hasSegment()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 * @exception OutOfRange Never thrown.
 */
Segment*
NullBus::segment(int) const
    throw (OutOfRange) {

    abortWithError("segment()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 * @exception InstanceNotFound Never thrown.
 */
Segment*
NullBus::segment(const std::string&) const
    throw (InstanceNotFound) {

    abortWithError("segment()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullBus::segmentCount() const {
    abortWithError("segmentCount()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullBus::isConnectedTo(const Socket&) const {
    abortWithError("isConnectedTo()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullBus::addGuard(Guard&)
    throw (ComponentAlreadyExists) {

    abortWithError("addGuard()");
}


/**
 * Aborts the program with error message.
 */
void
NullBus::removeGuard(Guard&) {
    abortWithError("removeGuard()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullBus::guardCount() const {
    abortWithError("guardCount()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 * @exception OutOfRange Never thrown.
 */
Guard*
NullBus::guard(int) const
    throw (OutOfRange) {

    abortWithError("guard()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullBus::hasNextBus() const {
    abortWithError("hasNextBus()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullBus::hasPreviousBus() const {
    abortWithError("hasPreviousBus()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 * @exception InstanceNotFound Never thrown.
 */
Bus*
NullBus::nextBus() const
    throw (InstanceNotFound) {

    abortWithError("nextBus()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 * @exception InstanceNotFound Never thrown.
 */
Bus*
NullBus::previousBus() const
    throw (InstanceNotFound) {

    abortWithError("previousBus()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullBus::canRead(const Bus&) const {
    abortWithError("canRead()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullBus::canWrite(const Bus&) const {
    abortWithError("canWrite()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullBus::canReadWrite(const Bus&) const {
    abortWithError("canReadWrite()");
    return false;
}


/**
 * Aborts the program with error message.
 */
void
NullBus::setSourceBridge(Bridge&) {
    abortWithError("setSourceBridge()");
}


/**
 * Aborts the program with error message.
 */
void
NullBus::setDestinationBridge(Bridge&) {
    abortWithError("setDestinationBridge()");
}


/**
 * Aborts the program with error message.
 */
void
NullBus::clearSourceBridge(Bridge&) {
    abortWithError("clearSourceBridge()");
}


/**
 * Aborts the program with error message.
 */
void
NullBus::clearDestinationBridge(Bridge&) {
    abortWithError("clearDestinationBridge()");
}


/**
 * Aborts the program with error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullBus::setMachine(Machine&)
    throw (ComponentAlreadyExists) {

    abortWithError("setMachine()");
}


/**
 * Aborts the program with error message.
 */
void
NullBus::unsetMachine() {
    abortWithError("unsetMachine()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
Machine*
NullBus::machine() const {
    abortWithError("machine()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception IllegalRegistration Never thrown.
 */
void
NullBus::ensureRegistration(const Component&) const
    throw (IllegalRegistration) {

    abortWithError("ensureRegistration()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullBus::isRegistered() const {
    abortWithError("isRegistered()");
    return false;
}

/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
ObjectState*
NullBus::saveState() const {
    abortWithError("saveState()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception ObjectStateLoadingException Never thrown.
 */
void
NullBus::loadState(const ObjectState*)
    throw (ObjectStateLoadingException) {

    abortWithError("loadState()");
}

}
