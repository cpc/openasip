/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file FUCollisionMatrixIndex.cc
 *
 * Definition of FUCollisionMatrixIndex class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen-no.spam-tut.fi)
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

