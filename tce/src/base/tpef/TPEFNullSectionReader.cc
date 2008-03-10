/**
 * @file TPEFNullSectionReader.cc
 *
 * Definitions of TPEFNullSectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <string>

#include "TPEFNullSectionReader.hh"
#include "SectionReader.hh"
#include "BaseType.hh"
#include "SafePointer.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::SectionIndexKey;
using std::string;

TPEFNullSectionReader TPEFNullSectionReader::proto_;

/**
 * Constructor.
 *
 * Registers itself to SectionReader.
 */
TPEFNullSectionReader::TPEFNullSectionReader() : TPEFSectionReader() {
    SectionReader::registerSectionReader(this);
}

/**
 * Destructor.
 */
TPEFNullSectionReader::~TPEFNullSectionReader() {
}

/**
 * Returns the type of section that reader can read.
 *
 * @return The type of section that reader can read.
 */
Section::SectionType
TPEFNullSectionReader::type() const {
    return Section::ST_NULL;
}

}
