/**
 * @file InstructionElement.hh
 *
 * Declaration of InstructionElement and InstructionAnnotation classes.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_INSTRUCTION_ELEMENT_HH
#define TTA_INSTRUCTION_ELEMENT_HH

#include "BaseType.hh"
#include "SectionElement.hh"

namespace TPEF {

/**
 * Instruction annotation.
 *
 * Inline annotations allow to argument an instruction element
 * with additional application-dependent information without
 * disrupting the normal work of application.
 */
class InstructionAnnotation {
public:
    InstructionAnnotation(Word anId, const std::vector<Byte>& payload);
    InstructionAnnotation(Word anId);
    ~InstructionAnnotation();

    Word id() const;

    Byte byte(Byte index) const;
    const std::vector<Byte>& payload() const;

    void addByte(Byte aByte);

    Byte size() const;

private:
    /// Maximum number of bytes that annotation may contain.
    static const int MAX_ANNOTATION_BYTES;
    /// Identifcation code of annotation.
    Word id_;
    /// Data of annotation.
    std::vector<Byte> payLoad_;
};

/**
 * Abstract base class for all instructions of CodeSection.
 */
class InstructionElement : public SectionElement {
public:
    virtual ~InstructionElement();

    bool isMove() const;
    bool isImmediate() const;

    bool begin() const;
    void setBegin(bool isBegin);

    InstructionAnnotation* annotation(Word index) const;
    void addAnnotation(InstructionAnnotation *anAnnotation);
    Word annotationCount() const;

protected:
    InstructionElement(bool isMove);

private:
    /// Is the first element of whole instruction.
    bool begin_;
    /// Is move or immediate.
    bool isMove_;

    /// All annotations.
    std::vector<InstructionAnnotation*> annotations_;
};
}

#include "InstructionElement.icc"

#endif
