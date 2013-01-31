/**
 * @file Provides the functional library.
 */

#pragma once
#include "sprite/sprite.hpp"

namespace sprite { namespace lib
{
  // This is a dummy type that is used only by apply to force its first
  // successor to be evaluated if it is an operation.
  #define SPRITE_LIB_Partial (lib, ((PartialHolder, "partialHolder", 0)))
  typedef sprite::Partial<sprite::FailNode,0> PartialHolder;


  // apply f x = f x
  // -- Evaluates f, as needed, before application.
  OPERATION(apply, "apply", 2
    , (DT_BRANCH, RDX[0], SPRITE_LIB_Partial
        // ??? FwdNode is a workaround !!!
        , (DT_LEAF, REWRITE(FwdNode, APPLY(RDX[0], RDX[1])))
        )
    )


  // compose   :: (b -> c) -> (a -> b) -> (a -> c)
  // compose f g = \x -> f (g x)
  // Denoted as (.)
  //
  // auxcomp f g x = f(g(x))
  OPERATION(auxcomp, "auxcomp", 3
    , (DT_LEAF, REWRITE(apply, RDX[0], NODE(apply, RDX[1], RDX[2])))
    )
  typedef Partial<auxcomp,2> compose;


  // const_ a _ = a
  // Denoted as const
  OPERATION(const_, "const", 2, (DT_LEAF, REWRITE(FwdNode, RDX[0])))
}}
