#pragma once
#include "sprite/sprite.hpp"

namespace rfib
{
  /*
    nfib :: Double -> Double
    nfib n = if n <= 1 then 1 else nfib (n-1) + nfib (n-2) + 1

    main = nfib 30
  */

  using namespace sprite;

  STATIC_NODE(one, IntNode, 1);
  STATIC_NODE(two, IntNode, 2);

  OPERATION(NFibNode, "nfib", 1
    , (DT_LEAF
        , COND(
              LeNode(RDX[0], one)
            , REWRITE(IntNode, 1)
            , REWRITE(
                  AddNode
                , NODE(
                      AddNode
                    , NODE(NFibNode, NODE(SubNode, RDX[0], one))
                    , NODE(NFibNode, NODE(SubNode, RDX[0], two))
                    )
                , one
                )
            )
        )
    )

  OPERATION(MainNode, "main", 0
    , (DT_LEAF, REWRITE(NFibNode, NODE(IntNode, 30)))
    )

}
