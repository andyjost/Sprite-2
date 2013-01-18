#pragma once
#include "sprite/sprite.hpp"
#include "sprite/lib/list.hpp"
#include "sprite/lib/int.hpp"

namespace sprite { namespace module { namespace test_map
{
  /*
    mymap f [] = []
    mymap f (h:t) = f h : mymap f t
    main = mymap (+1) [0,1]
  */

  using namespace sprite;
  using lib::constants::i0;
  using lib::constants::i1;
  using lib::Cons;
  using lib::Nil;
  using lib::nil;


  OPERATION(MyMapNode, "mymap", 2
    , (DT_BRANCH, RDX[1], PRELUDE_TP_LIST
        , (DT_LEAF, REWRITE(Nil))
        , (DT_LEAF
            , REWRITE(Cons, APPLY(RDX[0],IND[0]), NODE(MyMapNode,RDX[0],IND[1]))
            )
        )
    )


  OPERATION(MainNode, "main", 0
    , (DT_LEAF
        , REWRITE(
              MyMapNode
            , NODE((Partial<AddNode,1>), i1)
            , NODE(Cons, i0, NODE(Cons,i1,nil))
            )
        )
    )
}}}
