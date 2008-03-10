/**
 * @file LineNumProcedure.cc
 *
 * Non-inline definitions of LineNumProcedure class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "LineNumProcedure.hh"

namespace TPEF {

using ReferenceManager::SafePointer;

/**
 * Constructor.
 */
LineNumProcedure::LineNumProcedure() : SectionElement(),
    symbol_(&SafePointer::null) {
}

/**
 * Destructor.
 *
 * Deletes all line numbers inside procedure.
 */
LineNumProcedure::~LineNumProcedure() {
    for (HalfWord i = 0; i < lines_.size(); i++) {
        delete lines_[i];
        lines_[i] = NULL;
    }
    lines_.clear();
}

}
