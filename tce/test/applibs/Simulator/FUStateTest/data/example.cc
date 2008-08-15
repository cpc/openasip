/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file example.cc
 *
 * Operations for testing purposes.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 */

#include "OSAL.hh"

OPERATION(TESTADD)

TRIGGER
  IO(3) = INT(1) + INT(2);
  RETURN_READY;
END_TRIGGER;

END_OPERATION(TESTADD)

OPERATION(TESTSUB)

TRIGGER
  IO(3) = INT(1) - INT(2);
  RETURN_READY;
END_TRIGGER;

END_OPERATION(TESTSUB)

OPERATION(TESTMUL)

TRIGGER
  IO(3) = INT(1) * INT(2);
  RETURN_READY;
END_TRIGGER;

END_OPERATION(TESTMUL)

OPERATION(TESTSTORE)

TRIGGER
    MEMORY.write(INT(1), 4, UINT(2));
END_TRIGGER;

END_OPERATION(TESTSTORE)

OPERATION(TESTLOAD)

TRIGGER
  UIntWord data;
  MEMORY.read(INT(1), 4, data);
  IO(2) = data;
END_TRIGGER;

END_OPERATION(TESTLOAD)
