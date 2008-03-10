/**
 * @file LineReaderFactory.hh
 *
 * Declaration of LineReaderFactory class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_LINE_READER_FACTORY_HH
#define TTA_LINE_READER_FACTORY_HH

class LineReader;

/**
 * Factory for creating the best version of LineReader available.
 *
 * The best version of LineReader available is figured out during configure.
 */
class LineReaderFactory {
public:
    static LineReader* lineReader();
};

#endif
