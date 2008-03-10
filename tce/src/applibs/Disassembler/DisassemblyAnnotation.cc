/**
 * @file DisassemblyAnnotation.cc
 *
 * Definition of DisassemblyAnnotation class.
 *
 * @author Mikael Lepistö 2007 (tmlepist@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyAnnotation.hh"
#include "Conversion.hh"
#include "StringTools.hh"

using std::string;

/**
 * Constructor.
 *
 * @param id Id of the annotation.
 * @param payload Data for the annotation.
 */
DisassemblyAnnotation::DisassemblyAnnotation(
    Word id, const std::vector<Byte>& payload):
    DisassemblyElement(), id_(id), payload_(payload) {
}

/**
 * Destructor.
 */
DisassemblyAnnotation::~DisassemblyAnnotation() {
}


/**
 * Returns disassembly of the annotation.
 */
string
DisassemblyAnnotation::toString() const {

    std::stringstream retVal;
    retVal << "{" << std::hex << "0x" << id_;    

    for (unsigned int i = 0; i < payload_.size(); i++) {
        retVal << " " << int(payload_[i]);
    }

    retVal << "}";
    return retVal.str();
}
