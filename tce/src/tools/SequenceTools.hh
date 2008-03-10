/**
 * @file SequenceTools.hh
 * @author Pekka J‰‰skel‰inen (pekka.jaaskelainen@tut.fi) 2004
 *
 * Tools for inspecting and manipulating STL Sequences.
 *
 * Declarations.
 *
 */

#ifndef TTA_SEQUENCETOOLS_HH
#define TTA_SEQUENCETOOLS_HH

class SequenceTools {
public:
    template <typename SequenceType>
    static void deleteAllItems(SequenceType& aSequence);
};

#include "SequenceTools.icc"

#endif
