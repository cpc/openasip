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
 * OSAL behavior definition file.
 */


#include "OSAL.hh"

OPERATION(REFLECT8)
TRIGGER
unsigned long data = UINT(1);
unsigned char nBits = 8;

unsigned long  reflection = 0x00000000;
unsigned char  bit;
 /*
  * Reflect the data about the center bit.
  */
for (bit = 0; bit < nBits; ++bit)
{
    /*
     * If the LSB bit is set, set the reflection of it.
     */
    if (data & 0x01)
    {
        reflection |= (1 << ((nBits - 1) - bit));
    }
    
     data = (data >> 1);
}

IO(2) = static_cast<unsigned> (reflection);

return true;
END_TRIGGER;
END_OPERATION(REFLECT8)

OPERATION(REFLECT32)
TRIGGER
unsigned long data = UINT(1);
unsigned char nBits = 32;

unsigned long  reflection = 0x00000000;
unsigned char  bit;

/*
 * Reflect the data about the center bit.
 */
for (bit = 0; bit < nBits; ++bit)
{
    /*
     * If the LSB bit is set, set the reflection of it.
     */
    if (data & 0x01)
    {
        reflection |= (1 << ((nBits - 1) - bit));
    }
    
    data = (data >> 1);
 }

IO(2) = static_cast<unsigned> (reflection);

return true;
END_TRIGGER;
END_OPERATION(REFLECT32)
