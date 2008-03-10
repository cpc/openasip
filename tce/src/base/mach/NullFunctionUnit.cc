/**
 * @file NullFunctionUnit.cc
 *
 * Implementation of NullFunctionUnit class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: yellow
 */

#include "NullFunctionUnit.hh"

namespace TTAMachine {

NullFunctionUnit NullFunctionUnit::instance_;

/**
 * The constructor.
 */
NullFunctionUnit::NullFunctionUnit() : FunctionUnit("NULL") {
}


/**
 * The destructor.
 */
NullFunctionUnit::~NullFunctionUnit() {
}


/**
 * Returns the only instance of NullFunctionUnit.
 *
 * @return The only NullFunctionUnit instance.
 */
NullFunctionUnit&
NullFunctionUnit::instance() {
    return instance_;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
Machine*
NullFunctionUnit::machine() const {
    abortWithError("machine()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception IllegalRegistration Never thrown.
 */
void
NullFunctionUnit::ensureRegistration(const Component&) const
    throw (IllegalRegistration) {

    abortWithError("ensureRegistration()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullFunctionUnit::isRegistered() const {
    abortWithError("isRegistered()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
std::string
NullFunctionUnit::name() const {
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
NullFunctionUnit::setName(const std::string&)
    throw (ComponentAlreadyExists, InvalidName) {

    abortWithError("setName()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullFunctionUnit::hasPort(const std::string&) const {
    abortWithError("hasPort()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullFunctionUnit::portCount() const {
    abortWithError("portCount()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 * @exception InstanceNotFound Never thrown.
 */
BaseFUPort*
NullFunctionUnit::port(const std::string&) const
    throw (InstanceNotFound) {

    abortWithError("port()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 * @exception OutOfRange Never thrown.
 */
BaseFUPort*
NullFunctionUnit::port(int) const
    throw (OutOfRange) {

    abortWithError("port()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullFunctionUnit::operationPortCount() const {
    abortWithError("operationPortCount()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullFunctionUnit::hasOperationPort(const std::string&) const {
    abortWithError("hasOperationPort()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 * @exception InstanceNotFound Never thrown.
 */
FUPort*
NullFunctionUnit::operationPort(const std::string&) const
    throw (InstanceNotFound) {

    abortWithError("operationPort()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 * @exception OutOfRange Never thrown.
 */
FUPort*
NullFunctionUnit::operationPort(int) const
    throw (OutOfRange) {

    abortWithError("operationPort()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullFunctionUnit::addOperation(HWOperation&)
    throw (ComponentAlreadyExists) {

    abortWithError("addOperation()");
}


/**
 * Aborts the program with error message.
 *
 * @exception InstanceNotFound Never thrown.
 */
void
NullFunctionUnit::deleteOperation(HWOperation&)
    throw (InstanceNotFound) {

    abortWithError("deleteOperation()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullFunctionUnit::hasOperation(const std::string&) const {
    abortWithError("hasOperation()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 * @exception InstanceNotFound Never thrown.
 */
HWOperation*
NullFunctionUnit::operation(const std::string&) const
    throw (InstanceNotFound) {

    abortWithError("operation()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 * @exception OutOfRange Never thrown.
 */
HWOperation*
NullFunctionUnit::operation(int) const
    throw (OutOfRange) {

    abortWithError("operation()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullFunctionUnit::operationCount() const {
    abortWithError("operationCount()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullFunctionUnit::maxLatency() const {
    abortWithError("maxLatency()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullFunctionUnit::addPipelineElement(PipelineElement&)
    throw (ComponentAlreadyExists) {

    abortWithError("addPipelineElement()");
}


/**
 * Aborts the program with error message.
 */
void
NullFunctionUnit::deletePipelineElement(PipelineElement&) {
    abortWithError("deletePipelineElement()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullFunctionUnit::pipelineElementCount() const {
    abortWithError("pipelineElementCount()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 * @exception OutOfRange Never thrown.
 */
PipelineElement*
NullFunctionUnit::pipelineElement(int) const
    throw (OutOfRange) {

    abortWithError("pipelineElement()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullFunctionUnit::hasPipelineElement(const std::string&) const {
    abortWithError("hasPipelineElement()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 * @exception InstanceNotFound Never thrown.
 */
PipelineElement*
NullFunctionUnit::pipelineElement(const std::string&) const
    throw (InstanceNotFound) {

    abortWithError("pipelineElement()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
AddressSpace*
NullFunctionUnit::addressSpace() const {
    abortWithError("addressSpace()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception IllegalRegistration Never thrown.
 */
void
NullFunctionUnit::setAddressSpace(AddressSpace*)
    throw (IllegalRegistration) {

    abortWithError("setAddressSpace()");
}


/**
 * Aborts the program with error message.
 */
bool
NullFunctionUnit::hasAddressSpace() const {
    abortWithError("hasAddressSpace()");
    return 0;
}


/**
 * Aborts the program with error message.
 */
void
NullFunctionUnit::cleanup(const std::string&) {
    abortWithError("cleanup()");
}


/**
 * Aborts the program with error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullFunctionUnit::setMachine(Machine&)
    throw (ComponentAlreadyExists) {

    abortWithError("setMachine()");
}


/**
 * Aborts the program with error message.
 */
void
NullFunctionUnit::unsetMachine() {
    abortWithError("unsetMachine()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
ObjectState*
NullFunctionUnit::saveState() const {
    abortWithError("saveState()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception ObjectStateLoadingException Never thrown.
 */
void
NullFunctionUnit::loadState(const ObjectState*)
    throw (ObjectStateLoadingException) {

    abortWithError("loadState()");
}

}
