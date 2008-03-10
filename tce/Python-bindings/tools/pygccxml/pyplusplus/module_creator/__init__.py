# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

from creator import creator_t
from sort_algorithms import sort_classes as findout_desired_order
from call_policies_resolver import built_in_resolver_t

def create( decls, module_name ):
    maker = creator_t(decls, module_name)
    return maker.create()
