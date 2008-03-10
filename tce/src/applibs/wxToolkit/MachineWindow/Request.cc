/**
 * @file Request.cc
 *
 * Definition of Request class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 13 2004 by vpj, ll, jn, am
 */

#include "Request.hh"

/**
 * The Constructor.
 *
 * @param type Type of the Request.
 */
Request::Request(RequestType type): type_(type) {}

/**
 * The Destructor.
 */
Request::~Request() {}
