/**
 * @file CollisionMatrix.hh
 *
 * Declaration of CollisionMatrix class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_COLLISION_MATRIX_HH
#define TTA_COLLISION_MATRIX_HH

#include <vector>
#include <deque>

#include "BitMatrix.hh"

class ResourceVectorSet;
class FUReservationTableIndex;

/**
 * Represents a collision matrix.
 */
class CollisionMatrix : public BitMatrix {
public:

    CollisionMatrix(
        FUReservationTableIndex& reservationTables, std::size_t index);

    CollisionMatrix(std::size_t width, std::size_t height, bool value);

    virtual ~CollisionMatrix();

    virtual bool isCollision(std::size_t row, std::size_t column) const;
};

#endif
