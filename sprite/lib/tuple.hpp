/**
 * @file Provides the tuple library.
 */

#pragma once
#include "sprite/sprite.hpp"

namespace sprite { namespace lib
{
  // Generates a tuple type definition for arity n.
  #define SPRITE_LIB_Tuple(n) (lib, ((Tuple##n, "Tuple"#n, n)))

  // Instantiate tuples for arity n=0,1,..,SPRITE_ARITY_BOUND-1
  #define F(z,n,_) TYPE(SPRITE_LIB_Tuple(n))
  BOOST_PP_REPEAT(SPRITE_ARITY_BOUND,F,)
  #undef F
}}

