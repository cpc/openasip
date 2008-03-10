/**
 * @file AnnotatedInstructionElement.cc
 *
 * Implementation of AnnotatedInstructionElement class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#include <utility> // std::pair

#include "AnnotatedInstructionElement.hh"
#include "MapTools.hh"

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// AnnotatedInstructionElement
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
AnnotatedInstructionElement::AnnotatedInstructionElement() {
}

/**
 * Destructor.
 */
AnnotatedInstructionElement::~AnnotatedInstructionElement() {
}

/**
 * Adds an annotation to the instruction element.
 *
 * @param annotation The annotation to add. 
 */
void
AnnotatedInstructionElement::addAnnotation(
    const ProgramAnnotation& annotation) {
    annotations_.insert(
        std::pair<ProgramAnnotation::Id, ProgramAnnotation>(
            annotation.id(), annotation));
}

/**
 * Sets an annotation to the instruction element as the sole annotation of
 * that type.
 *
 * Removes all annotations with the same type before adding the annotation.
 *
 * @param annotation The annotation to set. 
 */
void
AnnotatedInstructionElement::setAnnotation(
    const ProgramAnnotation& annotation) {
    
    // hmm.. does multimap::erase(key) remove *all* elements with the given
    // key, or just first found?
    do {
        AnnotationIndex::iterator i = annotations_.find(annotation.id());
        if (i == annotations_.end())
            break;
        annotations_.erase(i);
    } while (true);
    annotations_.insert(
        std::pair<ProgramAnnotation::Id, ProgramAnnotation>(
            annotation.id(), annotation));
}

/**
 * Returns the annotation at the given index (and id).
 *
 * @param index The index of the annotation.
 * @param id The id of the annotation (optional).
 * @exception OutOfRange If the index is out of range.
 */
ProgramAnnotation
AnnotatedInstructionElement::annotation(
    int index, ProgramAnnotation::Id id) const
    throw (OutOfRange) {
    
    std::pair<AnnotationIndex::const_iterator, 
        AnnotationIndex::const_iterator> range(
        annotations_.end(), annotations_.end());

    if (id != ProgramAnnotation::ANN_UNDEF_ID)
        // set the iterators to point to the set of elements with the given id
        range = annotations_.equal_range(id); 
    else
        // go through all elements
        range = std::make_pair(annotations_.begin(), annotations_.end());

    AnnotationIndex::const_iterator i = range.first;
    for (int counter = 0; counter < index; ++counter) {
        ++i;
        if (i == annotations_.end())
            break;
    }

    if (i == annotations_.end())
        throw OutOfRange(__FILE__, __LINE__, __func__);

    return (*i).second;
}

/**
 * Returns the count of annotations (with the given id).
 *
 * @param id The id of the annotations to count (optional).
 * @return The count of annotations (with the given id).
 */
int
AnnotatedInstructionElement::annotationCount(ProgramAnnotation::Id id) const {
    if (id != ProgramAnnotation::ANN_UNDEF_ID)
        return static_cast<int>(annotations_.count(id));
    else
        return static_cast<int>(annotations_.size());
}

/**
 * Removes all annotations (with the given id).
 *
 * @param id The id of the annotations to remove (optional).
 */
void
AnnotatedInstructionElement::removeAnnotations(ProgramAnnotation::Id id) {

    if (id == ProgramAnnotation::ANN_UNDEF_ID) {
        annotations_.clear();
    }
    AnnotationIndex::iterator i = annotations_.find(id);
    while (i != annotations_.end()) {
        annotations_.erase(i);
        i = annotations_.find(id);
    }
}

/**
 * Returns true in case there's at least one annotation with the given id.
 *
 * @param id An annotation id.
 * @return True in case there's at least one annotation with the given id.
 */
bool
AnnotatedInstructionElement::hasAnnotations(ProgramAnnotation::Id id) const {
    return annotationCount(id) > 0;
}

} // namespace TTAProgram
