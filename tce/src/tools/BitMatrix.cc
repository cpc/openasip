/**
 * @file BitMatrix.cc
 *
 * Implementation of BitMatrix class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#include <sstream>
#include <cmath>

#include "BitMatrix.hh"
#include "BaseType.hh"

/**
 * Builds a bit matrix of given size and default content.
 *
 * @param width The width of the matrix.
 * @param height The height of the matrix.
 * @param initialValue The initial value for elements.
 */
BitMatrix::BitMatrix(int width, int height, bool initialValue) :
    rows_(height), columns_(width), wordsPerRow_(
        static_cast<int>(
            std::ceil(columns_ / float(sizeof(RowWord) * BYTE_BITWIDTH)))) {

    matrix_ = new RowWord[rows_ * wordsPerRow_];

    RowWord word = 0;
    if (initialValue) {
        word = ~word;
    }
    for (int row = 0; row < rows_; ++row) {
        for (int col = 0; col < wordsPerRow_; ++col) {
            matrix_[row * wordsPerRow_ + col] = word;
        }
    }
}

/**
 * Frees the bit matrix content storage.
 */
BitMatrix::~BitMatrix() {
    delete[] matrix_;
    matrix_ = NULL;
}

/**
 * Copy constructor.
 */
BitMatrix::BitMatrix(const BitMatrix& another) :
    rows_(another.rows_), columns_(another.columns_),
    wordsPerRow_(another.wordsPerRow_) {

    matrix_ = new RowWord[rows_ * wordsPerRow_];

    for (int row = 0; row < rows_; ++row) {
        for (int col = 0; col < wordsPerRow_; ++col) {
            matrix_[row * wordsPerRow_ + col] = 
                another.matrix_[row * wordsPerRow_ + col];
        }
    }
}

/**
 * Sets all bits to zero.
 */
void
BitMatrix::setAllToZero() {

    for (int row = 0; row < rows_; ++row) {
        for (int col = 0; col < wordsPerRow_; ++col) {
            matrix_[row * wordsPerRow_ + col] = 0;
        }
    }
}

/**
 * Sets all bits to one.
 */
void
BitMatrix::setAllToOne() {

    for (int row = 0; row < rows_; ++row) {
        for (int col = 0; col < wordsPerRow_; ++col) {
            matrix_[row * wordsPerRow_ + col] = ~0;
        }
    }
}


/**
 * ASCII representation of the bit matrix.
 *
 * For debugging and testing. 
 *
 * @return A string representation of the matrix.
 */
std::string
BitMatrix::toString() const {
    std::ostringstream s;
    for (int row = 0; row < rows_; ++row) {
        for (int col = 0; col < columns_; ++col) {
            s << bitAt(col, row) << " ";
        }
        s << std::endl;
    }
    return s.str();
}

/**
 * Compares two bit matrices..
 *
 * @param other The RT to compare to.
 * @return True in case all values in both reservation tables are equal and
 * the dimensions are the same.
 */
bool
BitMatrix::operator==(const BitMatrix& other) const {

    if (other.rows_ != rows_ || other.columns_ != columns_)
        return false;

    for (int row = 0; row < rows_; ++row) {
        for (int col = 0; col < columns_; ++col) {
            if (other.bitAt(col, row) != bitAt(col, row))
                return false;
        }
    }
    return true;
}

/**
 * Returns a textual description of the matrix suitable to be used 
 * as a dot edge/node label.
 *
 * @return A string describing the matrix.
 */
std::string
BitMatrix::toDotString() const {

    std::string theString = "";
    for (int row = 0; row < rows_; ++row) {
        for (int column = 0; column < columns_; 
             ++column) {            
            if (bitAt(column, row))
                theString += "1 ";
            else
                theString += "0 ";
        }
        theString += "\\n";
    }
    return theString;
}

/**
 * Returns true in case the given matrix is "smaller" than this one.
 *
 * The one that has the first non-equal non-zero matrix element is considered
 * larger. That is, the matrix is considered as a bit string. This is an
 * arbitrary ordering constraint to make it possible to store BitMatrices
 * in sets.
 *
 * @param rightHand The right hand side of the comparison.
 * @return True in case the the right hand is considered larger.
 */
bool
BitMatrix::operator<(const BitMatrix& rightHand) const {

    for (int row = 0; row < rows_; ++row) {
        for (int column = 0; column < columns_; ++column) {
            bool left = bitAt(column, row);
            bool right = rightHand.bitAt(column, row);
            if (left != right)
                return static_cast<int>(left) < static_cast<int>(right);
        }
    }
    // they are equal
    return false;
}
