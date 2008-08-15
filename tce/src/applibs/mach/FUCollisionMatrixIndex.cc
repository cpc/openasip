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

