/**
 * @file AOutSectionReader.cc
 *
 * Non-inline definitions of AOutSectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "AOutSectionReader.hh"

namespace TPEF {

/**
 * Constructor
 */
AOutSectionReader::AOutSectionReader() :
    SectionReader() {
}

/**
 * Destructor
 */
AOutSectionReader::~AOutSectionReader() {
}

/**
 * Returns binary reader instance which uses AOutSectionReader classes.
 *
 * @return Binary reader which uses AOutSectionReader classes.
 */
BinaryReader*
AOutSectionReader::parent() const {
    return AOutReader::instance();
}

}
