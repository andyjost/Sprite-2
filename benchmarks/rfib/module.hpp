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

  OPERATION(nfib, "nfib", 1
    , (DT_LEAF
        , IF(
              NODE(LeNode, RDX[0], i1)
            , THEN(IntNode, 1)
            , ELSE(
                  AddNode
                , NODE(
                      AddNode
                    , NODE(nfib, NODE(SubNode, RDX[0], i1))
                    , NODE(nfib, NODE(SubNode, RDX[0], i2))
                    )
                , i1
                )
            )
        )
    )

  #if 0
  // This demonstrates an unboxing optimization.
  namespace unboxed
  {
    inline int64 nfib(int64 n)
      { return n <= 1 ? 1 : (nfib(n-1) + nfib(n-2) + 1); }
  }

  OPERATION(nfib_unboxed, "nfib_unboxed", 1
    , (DT_BRANCH, RDX[0], SPRITE_LIB_UnboxedInt
        , (DT_LEAF, REWRITE(IntNode, unboxed::nfib(RDX[0]->value())))
        )
    )
  #endif

  OPERATION(MainNode, "main", 0
    , (DT_LEAF, REWRITE(nfib, NODE(IntNode, 35)))
    )
}}}
