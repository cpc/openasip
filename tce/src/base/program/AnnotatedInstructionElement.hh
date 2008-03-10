/**
 * @file AnnotatedInstructionElement.hh
 *
 * Declaration of AnnotatedInstructionElement class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen@tut.fi)
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
