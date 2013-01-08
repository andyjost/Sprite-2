#pragma once
#include "sprite/sprite.hpp"

namespace tak
{
  /*
    tak :: Int -> Int -> Int -> Int
    tak x y z = if x <= y then z
                else tak (tak (x-1) y z)
                         (tak (y-1) z x)
                         (tak (z-1) x y)
    goal1 = tak 27 16 8
    main = goal1
  */

  using namespace sprite;

  STATIC_NODE(one, IntNode, 1);

  OPERATION(TakNode, "tak", 3
    , (DT_LEAF
        , COND(
              LeNode(RDX[0], RDX[1])
            , REWRITE(FwdNode, RDX[2])
            , REWRITE(
                  TakNode
                , NODE(TakNode, NODE(SubNode, RDX[0], one), RDX[1], RDX[2])
                , NODE(TakNode, NODE(SubNode, RDX[1], one), RDX[2], RDX[0])
                , NODE(TakNode, NODE(SubNode, RDX[2], one), RDX[0], RDX[1])
                )
            )
        )
    )

  OPERATION(Goal1Node, "goal1", 0
    , (DT_LEAF
        , REWRITE(
              TakNode, NODE(IntNode, 27), NODE(IntNode, 16), NODE(IntNode, 8)
            )
        )
    )

  OPERATION(MainNode, "main", 0, (DT_LEAF, REWRITE(Goal1Node)))
}
