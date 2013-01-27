#pragma once
#include "sprite/sprite.hpp"
#include "sprite/lib/functional.hpp"
#include "sprite/lib/int.hpp"
#include "sprite/lib/list.hpp"

namespace sprite { namespace module { namespace test_concatMap
{
  /*
    main = concatMap (enumFromTo 0) [0,1,2]
    -- [0,0,1,0,1,2]
  */

  using namespace sprite;
  using namespace sprite::lib;


  OPERATION(MainNode, "main", 0
    , (DT_LEAF
        , REWRITE(
              apply
            , NODE(concatMap, NODE(PARTIAL(enumFromTo,1), i0))
            , NODE(Cons, i0, NODE(Cons, i1, NODE(Cons, i2, nil)))
            )
        )
    )
}}}
