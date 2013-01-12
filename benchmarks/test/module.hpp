#pragma once
#include "sprite/sprite.hpp"

namespace sprite { namespace module { namespace test
{
  /*
    head (x::_) = x
    main = head []
  */

  using namespace sprite;

  #define MODULE_4test_TP_6MyList                                  \
      (module::test, ((MyCons, "MyCons", 2))((MyNil, "MyNil", 0))) \
    /**/
  TYPE(MODULE_4test_TP_6MyList)

  OPERATION(HeadNode, "head", 1
    , (DT_BRANCH, RDX[0], MODULE_4test_TP_6MyList
        , (DT_LEAF, REWRITE(FwdNode, IND[0]))
        , DT_EXEMPT
        )
    )

  OPERATION(MainNode, "main", 0, (DT_LEAF, REWRITE(HeadNode, NODE(MyNil))))
}}}
