#pragma once
#include "sprite/sprite.hpp"
#include "sprite/lib/list.hpp"
#include "sprite/lib/bool.hpp"

namespace sprite { namespace module { namespace test_bool
{
  using namespace sprite;

  OPERATION(test_BoolAnd, "test_BoolAnd", 0
	  , (DT_LEAF
        , REWRITE(
              lib::Cons
            , NODE(lib::BoolAnd, lib::true_, lib::true_)
            , NODE(
                  lib::Cons
                , NODE(lib::BoolAnd, lib::true_, lib::false_)
                , NODE(
                      lib::Cons
                    , NODE(lib::BoolAnd, lib::false_, lib::true_)
                    , NODE(
                          lib::Cons
                        , NODE(lib::BoolAnd, lib::false_, lib::false_)
                        , lib::nil
                        )
                    )
                )
            )
        )
    )
  OPERATION(MainNode, "main", 0, (DT_LEAF, REWRITE(test_BoolAnd)))
}}}
