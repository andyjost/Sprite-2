/**                                                                             
 * @file Provides the Boolean library.                                             
 */

#pragma once
#include "sprite/sprite.hpp"

namespace sprite { namespace lib
{

  // The Boolean type is defined with the built-ins.


  // The static constant expressions true and false.
  STATIC_NODE(true_, BoolNode, true);
  STATIC_NODE(false_, BoolNode, false);


  // False && _ = False
  // True && x = x
  OPERATION(BoolAnd, "&&", 2
    , (DT_BRANCH, RDX[0], SPRITE_LIB_Bool
	      , (DT_LEAF, REWRITE(BoolNode, false))
	      , (DT_LEAF, REWRITE(FwdNode, RDX[1]))
        )
    )


  // False || x = x
  // True || _ = True
  OPERATION(BoolOr, "||", 2
    , (DT_BRANCH, RDX[0], SPRITE_LIB_Bool
	      , (DT_LEAF, REWRITE(FwdNode, RDX[1]))
	      , (DT_LEAF, REWRITE(BoolNode, true))
        )
    )


  // not False = True
  // not True = False
  OPERATION(BoolNot, "not", 1
    , (DT_BRANCH, RDX[0], SPRITE_LIB_Bool
	      , (DT_LEAF, REWRITE(BoolNode, true))
	      , (DT_LEAF, REWRITE(BoolNode, false))
        )
    )
}}
