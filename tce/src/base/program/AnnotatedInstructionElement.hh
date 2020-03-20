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
 * @file AnnotatedInstructionElement.hh
 *
 * Declaration of AnnotatedInstructionElement class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_ANNOTATED_INSTRUCTION_ELEMENT_HH
#define TTA_ANNOTATED_INSTRUCTION_ELEMENT_HH

#include <string>
#include <map>

#include "ProgramAnnotation.hh"
#include "Exception.hh"

class TCEString;

namespace TTAProgram {

/**
 * AnnotatedInstructionElements provide an interface for the classes that
 * can be attached an instruction annotation.
 *
 * Basically the TerminalImmediate and Move inherit this to provide such
 * capabilities.
 */
class AnnotatedInstructionElement {
public:
    AnnotatedInstructionElement();
    ~AnnotatedInstructionElement();

    void addAnnotation(const ProgramAnnotation& annotation);
    void setAnnotation(const ProgramAnnotation& annotation);
    ProgramAnnotation annotation(
        int index,
        ProgramAnnotation::Id id = ProgramAnnotation::ANN_UNDEF_ID) const;
    int annotationCount(
        ProgramAnnotation::Id id = ProgramAnnotation::ANN_UNDEF_ID) const;
    void removeAnnotations(
        ProgramAnnotation::Id id = ProgramAnnotation::ANN_UNDEF_ID);
    bool hasAnnotations(
        ProgramAnnotation::Id id = ProgramAnnotation::ANN_UNDEF_ID) const;
    bool hasAnnotation(ProgramAnnotation::Id id, const TCEString& data) const;

    void copyAnnotationsFrom(const AnnotatedInstructionElement& other);

private:
    /// a type for the container of the annotations
    typedef std::multimap<ProgramAnnotation::Id, ProgramAnnotation> 
    AnnotationIndex;

    /// container for annotations
    AnnotationIndex annotations_;
};

}

#endif
