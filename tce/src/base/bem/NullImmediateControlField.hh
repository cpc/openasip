/**
 * @file NullImmediateControlField.hh
 *
 * Declaration of NullImmediateControlField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_IMMEDIATE_CONTROL_FIELD_HH
#define TTA_NULL_IMMEDIATE_CONTROL_FIELD_HH

#include "ImmediateControlField.hh"

/**
 * A null class of ImmediateControlField.
 */
class NullImmediateControlField : public ImmediateControlField {
public:
    static NullImmediateControlField& instance();

private:
    NullImmediateControlField();
    virtual ~NullImmediateControlField();

    /// The only instance.
    static NullImmediateControlField field_;
    /// The parent binary encoding.
    static BinaryEncoding bem_;
};

#endif
