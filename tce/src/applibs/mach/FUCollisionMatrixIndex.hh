/**
 * @file FUCollisionMatrixIndex.hh
 *
 * Declaration of FUCollisionMatrixIndex class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_COLLISION_MATRIX_INDEX_HH
#define TTA_FU_COLLISION_MATRIX_INDEX_HH

#include <vector>

class CollisionMatrix;
namespace TTAMachine {
    class FunctionUnit;
}

/**
 * Represents an index of collision matrices required for modeling collision
 * behavior of operations in an FU.
 */
class FUCollisionMatrixIndex {
public:
    FUCollisionMatrixIndex(const TTAMachine::FunctionUnit& functionUnit);
    virtual ~FUCollisionMatrixIndex();

    CollisionMatrix& at(int index);
    int size() const;
private:
    /// Stores all collision matrices, including one for the pseudo NOP.
    std::vector<CollisionMatrix*> matrices_;
};
#endif
