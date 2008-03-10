/**
 * @file LineReaderFactory.cc
 *
 * Declaration of LineReaderFactory class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "LineReaderFactory.hh"
#include "LineReader.hh"

#include "config.h"

#ifdef EDITLINE
#include "EditLineReader.hh"
#else
#include "BaseLineReader.hh"
#endif


/**
 * Creates the best possible LineReader available in the system.
 *
 * Returned instance is owned by the client. The best possible LineReader
 * depends on which libraries are installed in the system.
 * 
 * @return A LineReader instance.
 */
LineReader*
LineReaderFactory::lineReader() {
#ifdef EDITLINE
    return new EditLineReader();
#else
    return new BaseLineReader();
#endif
}
