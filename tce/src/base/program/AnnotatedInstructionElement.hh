/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
    virtual ~AnnotatedInstructionElement();

    virtual void addAnnotation(const ProgramAnnotation& annotation);
    virtual void setAnnotation(const ProgramAnnotation& annotation);
    virtual ProgramAnnotation annotation(
        int index, ProgramAnnotation::Id id = ProgramAnnotation::ANN_UNDEF_ID)
        const throw (OutOfRange);
    virtual int annotationCount(
        ProgramAnnotation::Id id = ProgramAnnotation::ANN_UNDEF_ID) const;
    virtual void removeAnnotations(
        ProgramAnnotation::Id id = ProgramAnnotation::ANN_UNDEF_ID);
    virtual bool hasAnnotations(
        ProgramAnnotation::Id id = ProgramAnnotation::ANN_UNDEF_ID) const;
private:
    /// a type for the container of the annotations
    typedef std::multimap<ProgramAnnotation::Id, ProgramAnnotation> 
    AnnotationIndex;

    /// container for annotations
    AnnotationIndex annotations_;
};

}

#endif
