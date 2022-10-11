# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""This package defines all user messages( warnings + errors ), which will be 
reported to user.
"""
from warnings_ import *

#implementation using regular expression is deprecated, I will leave it here for 
#some time to be sure that the new one does not cause any problems.
#import re
#__RE_GET_WARNING_ID = re.compile( r'warning\s(?P<id>W(\d){4})' )
#match_obj = __RE_GET_WARNING_ID.search(msg)
# if not match_obj:
#     return None
# else:
#     return match_obj.group( 'id' )

def find_out_message_id( msg ):
    return msg.identifier

DISABLE_MESSAGES = [
    W1000, W1001, W1002, W1011, W1012, W1013, W1015, W1019, W1030, W1034, W1039
]
#Messages kept by DISABLE_MESSAGES list will not be reported

def disable( *args ):
    DISABLE_MESSAGES.extend( args )

def filter_disabled_msgs( msgs, disable_messages=None ):
    report = []
    
    skip_them = DISABLE_MESSAGES[:]
    if disable_messages:
        skip_them.extend( disable_messages )
    
    skip_them = filter( None, map( find_out_message_id, skip_them ) )

    for msg in msgs:
        msg_id = find_out_message_id( msg )
        if msg_id and msg_id not in skip_them:
            report.append( msg )

    return report





