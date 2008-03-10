/**
 * @file BitMatrix.hh
 *
 * Declaration of BitMatrix class
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BIT_MATRIX_HH
#define TTA_BIT_MATRIX_HH

/**
 * Models a bit matrix.
 *
 * The implementation should be quite efficient. It uses natural words of
 * the machine to store the bits to provide fast logical operations on
 * larger bit chunks at the same time. Especially functions such as fully ORing
 * and ANDing two bit matrices should be quite efficient. Checking a single
 * matrix element is couple of cycles slower than the naive implementation
 * of storing one bit per word. The main methods are inline non-virtual
 * methods for fastest possible access.
 */
class BitMatrix {
public:
    BitMatrix(int width, int height, bool initialValue);
    BitMatrix(const BitMatrix& another);
    virtual ~BitMatrix();

    bool bitAt(int column, int row) const;
    void setBit(int column, int row, bool value);
    void setAllToZero();
    void setAllToOne();
    void shiftLeft();
    void orWith(const BitMatrix& another);
    bool conflictsWith(const BitMatrix& another) const;
    
    int rowCount() const;
    int columnCount() const;

    std::string toString() const;
    std::string toDotString() const;

    bool operator==(const BitMatrix& other) const;
    bool operator<(const BitMatrix& rightHand) const;
private:
    // Storage for bits of a row in the reservation table.
    typedef unsigned RowWord;
    // The contents of the matrix.
    RowWord* matrix_;
    // Count of rows in the matrix.
    const int rows_;
    // Count of columns in the matrix.
    const int columns_;
    // Count of words per row to store the bits.
    const int wordsPerRow_;   
};

#include "BitMatrix.icc"

#endif
