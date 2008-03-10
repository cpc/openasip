/**
 * @file InstructionElement.cc
 *
 * Non-inline definitions of InstructionElement class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "InstructionElement.hh"

namespace TPEF {

//////////////////////////////////////////////////////////////////////////////
// InstructionAnnotation definition.
//////////////////////////////////////////////////////////////////////////////

/// Maximum number of bytes in annotation.
const int InstructionAnnotation::MAX_ANNOTATION_BYTES = 131;

/**
 * Constructor.
 *
 * @param id The id of the annotation.
 * @param payload The payload data as a char vector.
 */
InstructionAnnotation::InstructionAnnotation(
    Word id, const std::vector<Byte>& payload) : 
    id_(id), payLoad_(payload) {
}


/**
 * Constructor
 */
InstructionAnnotation::InstructionAnnotation(Word anId) : id_(anId) {
}

/**
 * Destructor
 */
InstructionAnnotation::~InstructionAnnotation() {
}

//////////////////////////////////////////////////////////////////////////////
// InstructionElement definition.
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
InstructionElement::InstructionElement(bool isMove) :
    SectionElement(), begin_(false), isMove_(isMove) {
}

/**
 * Destructor
 */
InstructionElement::~InstructionElement() {
    for (Word i = 0; i < annotationCount(); i++) {
        delete annotation(i);
    }
}

}
