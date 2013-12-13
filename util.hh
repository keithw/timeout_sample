/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef UTIL_HH
#define UTIL_HH

#include <string>
#include <cstring>

#include "address.hh"
#include "poller.hh"

template <typename T> void zero( T & x ) { memset( &x, 0, sizeof( x ) ); }

#endif /* UTIL_HH */
