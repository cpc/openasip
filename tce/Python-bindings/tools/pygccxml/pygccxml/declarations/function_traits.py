# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""
defines few algorithms, that deals with different properties of functions
"""

import calldef
import type_traits

def is_same_function( f1, f2 ):
    """returns true if f1 and f2 is same function

    Use case: sometimes when user defines some virtual function in base class,
    it overrides it in a derived one. Sometimes we need to know whether two member
    functions is actualy same function.
    """
    if f1 is f2:
        return True
    if not f1.__class__ is f2.__class__:
        return False
    if isinstance( f1, calldef.member_calldef_t ) and f1.has_const != f2.has_const:
        return False
    if f1.name != f2.name:
        return False
    if not type_traits.is_same( f1.return_type, f2.return_type):
        return False
    if len( f1.arguments ) != len(f2.arguments):
        return False
    for f1_arg, f2_arg in zip( f1.arguments, f2.arguments ):
        if not type_traits.is_same( f1_arg.type, f2_arg.type ):
            return False
    return True



