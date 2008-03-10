/**
 * @file NullMoveSlot.hh
 *
 * Declaration of NullMoveSlot class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_MOVE_SLOT_HH
#define TTA_NULL_MOVE_SLOT_HH

#include "MoveSlot.hh"

/**
 * A null move slot.
 */
class NullMoveSlot : public MoveSlot {
public:
    static NullMoveSlot& instance();


private:
    NullMoveSlot();
    virtual ~NullMoveSlot();
};

#endif
