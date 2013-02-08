#pragma once
#include "sprite/sprite.hpp"
#include "sprite/lib/int.hpp"

namespace sprite { namespace module { namespace tak
{
  /*
    tak :: Int -> Int -> Int -> Int
    tak x y z = if x <= y then z
                else tak (tak (x-1) y z)
                         (tak (y-1) z x)
                         (tak (z-1) x y)
    goal1 = tak 27 16 8
    main = do
      print $ goal1
  */

  using namespace sprite;
  using namespace lib::constants;

  OPERATION(TakNode, "tak", 3
    , (DT_LEAF
        , IF(
              NODE(LeNode, RDX[0], RDX[1])
            , THEN(FwdNode, RDX[2])
            , ELSE(
                  TakNode
                , NODE(TakNode, NODE(SubNode, RDX[0], i1), RDX[1], RDX[2])
                , NODE(TakNode, NODE(SubNode, RDX[1], i1), RDX[2], RDX[0])
                , NODE(TakNode, NODE(SubNode, RDX[2], i1), RDX[0], RDX[1])
                )
            )
        )
    )

  OPERATION(Goal1Node, "goal1", 0 ,(DT_LEAF, REWRITE(TakNode, i27, i16, i8)))
  OPERATION(MainNode, "main", 0, (DT_LEAF, REWRITE(Goal1Node)))
}}}
