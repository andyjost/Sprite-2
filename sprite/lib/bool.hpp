/**                                                                             
 * @file Provides the Boolean library.                                             
 */

#pragma once
#include "sprite/sprite.hpp"

namespace sprite { namespace lib
{

  // The Boolean type.
  // #define PRELUDE_LIB_Bool (lib, ((true_, "True", 0))((false_, "False", 0)))
  // TYPE(PRELUDE_LIB_Bool)

  // The static constant expression true and false.
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

  // not False = True
  // not True = False
  OPERATION(BoolNot, "not", 1
    , (DT_BRANCH, RDX[0], SPRITE_LIB_Bool
	      , (DT_LEAF, REWRITE(BoolNode, true))
	      , (DT_LEAF, REWRITE(BoolNode, false))
        )
    )

  // if False then _ else y = y
  // if True  then x else _ = x
  OPERATION(ifThenElse, "ifThenElse", 3
    , (DT_BRANCH, RDX[0], SPRITE_LIB_Bool
	      , (DT_LEAF, REWRITE(FwdNode, RDX[2]))
	      , (DT_LEAF, REWRITE(FwdNode, RDX[1]))
        )
    )
}}