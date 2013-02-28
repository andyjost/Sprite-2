#pragma once
#include "sprite/sprite.hpp"
#include "sprite/lib/int.hpp"

namespace sprite { namespace module { namespace rfibn
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

  namespace eager
  {
    inline int64 nfib(int64 n)
      { return n <= 1 ? 1 : (nfib(n-1) + nfib(n-2) + 1); }
  }

  OPERATION(nfib, "nfib", 1
    , (DT_BRANCH, RDX[0], SPRITE_LIB_UnboxedInt
        , (DT_LEAF,
              NodePtr result;

              BEGIN_EAGER
                result = box(eager::nfib(unbox(RDX[0])));
              END_EAGER

              REWRITE(FwdNode, result);
            )
        )
    )

  OPERATION(MainNode, "main", 0
    , (DT_LEAF, REWRITE(nfib, NODE(IntNode, 35)))
    )
}}}
