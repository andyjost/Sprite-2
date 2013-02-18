#pragma once

#include "sprite/node.hpp"
#include "sprite/evaluation.hpp"

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
}
