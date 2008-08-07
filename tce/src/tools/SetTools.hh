/**
 * @file SetTools.hh
 *
 * Tools for handling STL Sets.
 *
 * Declarations.
 *
 * @author Heikki Kultala 2008 (hkultala@cs.tut.fi)
 */

#ifndef TTA_SET_TOOLS_HH
#define TTA_SET_TOOLS_HH

#include <set>
#include <utility>

#include "Exception.hh"

class SetTools {
public:
    template <typename ValueType>
    static void intersection(
        const std::set<ValueType>& firstContainer,
        const std::set<ValueType>& secondContainer,
        std::set<ValueType>& intersection);

    template <typename ValueType, typename Comparator>
    static void intersection(
        const std::set<ValueType, Comparator>& firstContainer,
        const std::set<ValueType, Comparator>& secondContainer,
        std::set<ValueType, Comparator>& intersection);
};

#include "SetTools.icc"
#endif
