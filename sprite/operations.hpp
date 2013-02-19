#pragma once

#include "sprite/node.hpp"

namespace sprite
{
  // ==== Math Operations ====
  #define SPRITE_BINOP_DECL(name, type, op)                           \
      OPERATION(name, #op, 2                                          \
        , (DT_BRANCH, RDX[0], SPRITE_LIB_UnboxedInt                   \
        , (DT_BRANCH, RDX[1], SPRITE_LIB_UnboxedInt                   \
        , (DT_LEAF, REWRITE(type, RDX[0]->value() op RDX[1]->value()) \
        ))))                                                          \
    /**/
  
  SPRITE_BINOP_DECL(AddNode, IntNode, +)
  SPRITE_BINOP_DECL(SubNode, IntNode, -)
  SPRITE_BINOP_DECL(MulNode, IntNode, *)
  SPRITE_BINOP_DECL(DivNode, IntNode, /)
  SPRITE_BINOP_DECL(ModNode, IntNode, %)
  SPRITE_BINOP_DECL(LtNode, BoolNode, <)
  SPRITE_BINOP_DECL(GtNode, BoolNode, >)
  SPRITE_BINOP_DECL(LeNode, BoolNode, <=)
  SPRITE_BINOP_DECL(GeNode, BoolNode, >=)
  SPRITE_BINOP_DECL(EqNode, BoolNode, ==)
  SPRITE_BINOP_DECL(NeNode, BoolNode, !=)

  namespace lib
  {
    // test True = True
    // test False = False
    // -- Tests the true/false value of x and forces the result to be exactly
    // -- True or False (e.g., removes Fwd nodes).
    OPERATION(test, "test", 1
      , (DT_BRANCH, RDX[0], SPRITE_LIB_Bool
          , (DT_LEAF, REWRITE(BoolNode, false))
          , (DT_LEAF, REWRITE(BoolNode, true))
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
  }
}
