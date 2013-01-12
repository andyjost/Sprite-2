/**
 * @file Provides the integer library.
 */

#pragma once
#include "sprite/sprite.hpp"

namespace sprite { namespace lib
{
  namespace constants
  {
    // Declare static integer constants i0, i1, ..., i64
    #define F(z,n,_) STATIC_NODE(i##n, IntNode, n);
    BOOST_PP_REPEAT(65,F,)
    #undef F
  }
}}}
