#pragma once
#include "sprite/sprite.hpp"
#include "sprite/lib/int.hpp"

namespace sprite { namespace module { namespace rfib
{
  /*
    nfib :: Int -> Int
    nfib n = if n <= 1 then 1 else nfib (n-1) + nfib (n-2) + 1

    main = do
      print $ nfib 35
  */

  using namespace sprite;
  using lib::constants::i1;
  using lib::constants::i2;

  OPERATION(NFibNode, "nfib", 1
    , (DT_LEAF
        , IF(
              NODE(LeNode, RDX[0], i1)
            , THEN(IntNode, 1)
            , ELSE(
                  AddNode
                , NODE(
                      AddNode
                    , NODE(NFibNode, NODE(SubNode, RDX[0], i1))
                    , NODE(NFibNode, NODE(SubNode, RDX[0], i2))
                    )
                , i1
                )
            )
        )
    )

  OPERATION(MainNode, "main", 0
    , (DT_LEAF, REWRITE(NFibNode, NODE(IntNode, 35)))
    )
}}}
