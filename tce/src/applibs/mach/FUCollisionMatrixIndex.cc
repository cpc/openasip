/**
 * @file FUCollisionMatrixIndex.cc
 *
 * Definition of FUCollisionMatrixIndex class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#include "FUCollisionMatrixIndex.hh"
#include "CollisionMatrix.hh"
#include "FUReservationTableIndex.hh"
#include "FunctionUnit.hh"
#include "SequenceTools.hh"

/**
 * Builds collision matrices for all operations in the FU and an empty
 * collision matrix for the pseudo no-operation.
 *
 * Matrices are stored in the order returned by FunctionUnit::operation(),
 * the NOP matrix is at index FunctionUnit::operationCount().
 *
 * @param functionUnit The FU to build the matrices for.
 */
FUCollisionMatrixIndex::FUCollisionMatrixIndex(
    const TTAMachine::FunctionUnit& functionUnit) {

    FUReservationTableIndex tables(functionUnit);
    int width = 0;
    int height = 0;
    for (int i = 0; i < functionUnit.operationCount(); ++i) {

        CollisionMatrix* matrix = new CollisionMatrix(tables, i);

#ifndef NDEBUG
        if (width == 0 && height == 0) {
            width = matrix->columnCount();
            height = matrix->rowCount();
        } else {
            // all matrices should be of the same dimension, otherwise there's
            // a bug somewhere
            assert(matrix->columnCount() == width);
            assert(matrix->rowCount() == height);
        }
#endif
        matrices_.push_back(matrix);
    }

    // The NOP.
    matrices_.push_back(new CollisionMatrix(width, height, false));
}

/**
 * Destructor.
 */
FUCollisionMatrixIndex::~FUCollisionMatrixIndex() {
    SequenceTools::deleteAllItems(matrices_);
}

/**
 * Returns the collision matrix at the given index.
 *
 * @param operation The index of the operat
 */
CollisionMatrix&
FUCollisionMatrixIndex::at(int index) {
    return *matrices_.at(index);
}

/**
 * Returns the count of collision matrices in the index.
 *
 * @return Count of collision matrices.
 */
int
FUCollisionMatrixIndex::size() const {
    return matrices_.size();
}

