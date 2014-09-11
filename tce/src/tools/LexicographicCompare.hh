/*
 * LexicographicCompare.hh
 *
 *  Created on: 11.9.2014
 *      Author: multanej
 */

#ifndef LEXICOGRAPHICCOMPARE_HH_
#define LEXICOGRAPHICCOMPARE_HH_

#include "NetlistBlock.hh"

namespace ProGe{
/**
     * Lexicographical comparison of 2 strings.
     *
     * @return true if a comes before b in dictionary order.
     */
    class LexicographicCompare
    {
    public:
        bool operator () (const NetlistBlock* a, const NetlistBlock* b) const{

        return (a->instanceName() + a->moduleName()) < (b->instanceName() + b->moduleName());
        }
    };
}
#endif /* LEXICOGRAPHICCOMPARE_HH_ */
