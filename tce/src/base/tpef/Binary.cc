/**
 * @file Binary.cc
 *
 * Non-inline definitions of Binary class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <iterator>
#include <algorithm>
#include <list>
#include <map>

#include "Binary.hh"
#include "Section.hh"

namespace TPEF {

using std::insert_iterator;
using std::copy;
using std::list;
using std::map;
using ReferenceManager::SafePointer;

/**
 * Constructor.
 */
Binary::Binary() :
    strings_(&SafePointer::null) {
}

/**
 * Destructor.
 */
Binary::~Binary() {
    while (sections_.size() != 0) {
        delete sections_[sections_.size() - 1];
        sections_[sections_.size() - 1] = NULL;
        sections_.pop_back();
    }
}

}
