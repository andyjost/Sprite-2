#pragma once
#include "sprite/sprite.hpp"

namespace test
{
  /*
    head (x::_) = x
    main = head []
  */

  using namespace sprite;

  #define TP_MyList ((MyCons, "MyCons", 2))((MyNil, "MyNil", 0))
  TYPE(TP_MyList)

  OPERATION(HeadNode, "head", 1
    , (DT_BRANCH, RDX[0], TP_MyList
        , (DT_LEAF, REWRITE(FwdNode, IND[0]))
        , DT_EXEMPT
        )
    )

  OPERATION(MainNode, "main", 0, (DT_LEAF, REWRITE(HeadNode, NODE(MyNil))))
}
