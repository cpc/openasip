/**
 * @file ProgramAnnotation.cc
 *
 * Implementation of ProgramAnnotation class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#include "ProgramAnnotation.hh"

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// ProgramAnnotation
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param id The id of the annotation (a 24-bit value).
 * @param data The payload data as a string.
 */
ProgramAnnotation::ProgramAnnotation(Id id, const std::string& data) :
    id_(id) {
    payload_ = std::vector<Byte>(data.begin(), data.end());
}

/**
 * Constructor.
 *
 * @param id The id of the annotation (a 24-bit value).
 * @param data The payload data as a byte vector.
 */
ProgramAnnotation::ProgramAnnotation(
    Id id, const std::vector<Byte>& payload) :
    id_(id), payload_(payload) {
}

/**
 * Destructor.
 */
ProgramAnnotation::~ProgramAnnotation() {
}

/**
 * The payload data as a std::string.
 *
 * @returns The data as a std::string.
 */
std::string
ProgramAnnotation::stringValue() const {
    return std::string(payload_.begin(), payload_.end());
}

/**
 * Returns the id of the annotation.
 *
 * @return the id of the annotation.
 */
ProgramAnnotation::Id
ProgramAnnotation::id() const {
    return id_;
}

/**
 * Return the payload data as a non-mutable char vector.
 *
 * @return The payload data.
 */
const std::vector<Byte>&
ProgramAnnotation::payload() const {
    return payload_;
}

} // namespace TTAProgram
