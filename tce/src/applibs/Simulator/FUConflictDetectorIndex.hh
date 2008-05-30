/**
 * @file FUConflictDetectorIndex.hh
 *
 * Declaration of FUConflictDetectorIndex type.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_CONFLICT_DETECTOR_INDEX_HH
#define TTA_FU_CONFLICT_DETECTOR_INDEX_HH

#include "FUResourceConflictDetector.hh"
#include <map>

/**
 * An index for FUResourceConflictDetectors
 */
typedef std::map<std::string, FUResourceConflictDetector*> 
FUConflictDetectorIndex;

#endif
