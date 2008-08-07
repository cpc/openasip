/**
 * @file AssocTools.hh
 * @author Pekka J‰‰skel‰inen (pekka.jaaskelainen@tut.fi) 2004
 *
 * Tools for handling STL Associative Containers (usually set).
 *
 * Declarations.
 *
 */

#ifndef TTA_ASSOC_TOOLS_HH
#define TTA_ASSOC_TOOLS_HH

#include <set>
#include <utility>

#include "Exception.hh"

class AssocTools {
public:
    template <typename ContainerType>
    static void deleteAllItems(ContainerType& aMap);

    template <typename ContainerType>
    static void deleteAllValues(ContainerType& aMap);

    template <typename ContainerType, typename KeyType>
    static bool containsKey(
        const ContainerType& aContainer,
        const KeyType& aKey);

    template <typename ContainerType>
    static void difference(
        const ContainerType& firstContainer,
        const ContainerType& secondContainer,
        ContainerType& difference);

    template <typename ContainerType1, typename ContainerType2>
    static std::set<std::pair<
        typename ContainerType1::value_type, 
        typename ContainerType2::value_type> > pairs(
        ContainerType1& firstContainer,
        ContainerType2& secondContainer);

    template <typename Comparator,typename ContainerType1, 
              typename ContainerType2> 
    static std::set<std::pair<
                        typename ContainerType1::value_type, 
        typename ContainerType2::value_type>, Comparator > pairs(
        ContainerType1& firstContainer,
        ContainerType2& secondContainer);
        
    template <typename ContainerType>
    static void append(const ContainerType& src, ContainerType &dest);
};

#include "AssocTools.icc"

#endif
