#pragma once
#include "sprite/sprite.hpp"
#include "sprite/lib/list.hpp"
#include "sprite/lib/int.hpp"

namespace sprite { namespace module { namespace filter_example
{
  /*
    main = filter (== 1) [0,1,0,1,2]
  */

  using namespace sprite;
  using lib::constants::i0;
  using lib::constants::i1;
  using lib::constants::i2;
  using lib::Cons;
  using lib::nil;


  OPERATION(MainNode, "main", 0
    , (DT_LEAF
        , REWRITE(
              lib::filter
            , NODE(PARTIAL(EqNode,1), i1)
            , NODE(Cons, i0, NODE(Cons, i1, NODE(Cons, i0
                , NODE(Cons, i1, NODE(Cons, i2, nil))))
                )
            )
        )
    )
}}}
