/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file AnnotatedInstructionElement.cc
 *
 * Implementation of AnnotatedInstructionElement class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#include <utility> // std::pair

#include "AnnotatedInstructionElement.hh"
#include "MapTools.hh"
#include "TCEString.hh"

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
    // multimap::erase(key) returns the number of elements removed as
    // size_type, so it erases all matching elements
    annotations_.erase(annotation.id());
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
    int index, ProgramAnnotation::Id id) const {
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

/**
 * Returns true in case there's at least one annotation with the given id
 * and the given data.
 */
bool
AnnotatedInstructionElement::hasAnnotation(
    ProgramAnnotation::Id id, const TCEString& data) const {

    auto range = annotations_.equal_range(id);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.stringValue() == data) {
            return true;
        }
    }
    return false;
}


/**
 * Copies annotations from another annotated element.
 *
 * Possible old annotations are deleted.
 */
void
AnnotatedInstructionElement::copyAnnotationsFrom(
    const AnnotatedInstructionElement& other) {
    annotations_ = other.annotations_;
}

} // namespace TTAProgram
