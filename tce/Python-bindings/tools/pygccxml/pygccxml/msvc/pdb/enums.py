# Copyright (c) 2002-2009 Tampere University.
#
# This file is part of TTA-Based Codesign Environment (TCE).
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
from ... import utils #import utils from pygccxml package

class BasicType(utils.enum):
    btNoType   = 0
    btVoid     = 1
    btChar     = 2
    btWChar    = 3
    btInt      = 6
    btUInt     = 7
    btFloat    = 8
    btBCD      = 9
    btBool     = 10
    btLong     = 13
    btULong    = 14
    btCurrency = 25
    btDate     = 26
    btVariant  = 27
    btComplex  = 28
    btBit      = 29
    btBSTR     = 30
    btHresult  = 31




#Adding code, that was not generated for some reason.
class UdtKind(utils.enum):
   UdtStruct, UdtClass, UdtUnion = (0, 1, 2)

class CV_access_e(utils.enum):
   CV_private, CV_protected, CV_public = (1, 2, 3)

class NameSearchOptions(utils.enum):
   nsNone               = 0
   nsfCaseSensitive     = 0x1
   nsfCaseInsensitive   = 0x2
   nsfFNameExt          = 0x4
   nsfRegularExpression = 0x8
   nsfUndecoratedName   = 0x10

   # For backward compabibility:
   nsCaseSensitive           = nsfCaseSensitive
   nsCaseInsensitive         = nsfCaseInsensitive
   nsFNameExt = nsfFNameExt
   nsRegularExpression       = nsfRegularExpression | nsfCaseSensitive
   nsCaseInRegularExpression = nsfRegularExpression | nsfCaseInsensitive


class DataKind( utils.enum ):
   DataIsUnknown        = 0
   DataIsLocal          = 1
   DataIsStaticLocal    = 2
   DataIsParam          = 3
   DataIsObjectPtr      = 4
   DataIsFileStatic     = 5
   DataIsGlobal         = 6
   DataIsMember         = 7
   DataIsStaticMember   = 8
   DataIsConstant       = 9

