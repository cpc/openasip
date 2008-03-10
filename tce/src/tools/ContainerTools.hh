/**
 * @file ContainerTools.hh
 * @author Pekka J‰‰skel‰inen (pjaaskel@cs.tut.fi) 2003
 *
 * Tools for handling STL containers.
 *
 * Declarations.
 *
 */

#ifndef TTA_CONTAINERTOOLS_HH
#define TTA_CONTAINERTOOLS_HH

class ContainerTools {
public:
    template <typename ContainerType, typename ElementType>
    static bool containsValue(
	const ContainerType& aContainer, const ElementType& aKey);

    template <typename ContainerType, typename ElementType>
    static bool removeValueIfExists(
        ContainerType& aContainer, const ElementType& aKey);

    template <typename ContainerType, typename ElementType>
    static bool deleteValueIfExists(
        ContainerType& aContainer, const ElementType& aKey);

    template <typename ContainerType>
    static void removeValues(
        ContainerType& aContainer,
        const ContainerType& toRemove);

};

#include "ContainerTools.icc"

#endif
