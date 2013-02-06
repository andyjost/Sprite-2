#pragma once
#include "test.hpp"
using namespace sprite::lib::constants;

// head [] -> failure
OPERATION(test_headfail, "test_headfail", 0, (DT_LEAF, REWRITE(lib::head, NODE(lib::Nil))))

// tail [] -> failure
OPERATION(test_tailfail, "test_tailfail", 0, (DT_LEAF, REWRITE(lib::tail, NODE(lib::Nil))))

// take 4 [1..] -> [1,2,3,4]
OPERATION(test_enumFrom, "test_enumFrom", 0
	  , (DT_LEAF, REWRITE(lib::take, i4, NODE(lib::enumFrom, i1)))
    )

// [4..8] -> [4,5,6,7,8]
OPERATION(test_enumFromTo, "test_enumFromTo", 0
	  , (DT_LEAF, REWRITE(lib::enumFromTo, i4, i8))
    )

// head (tail [2..]) -> 3
OPERATION(test_headtail, "test_headtail", 0
	  , (DT_LEAF, REWRITE(lib::head, NODE(lib::tail, NODE(lib::enumFrom, i2))))
    )

// filter (== 1) [0,1,0,1,2] -> [1,1]
OPERATION(test_filter, "test_filter", 0
  , (DT_LEAF
      , REWRITE(
            lib::filter
          , NODE(PARTIAL(EqNode,1), i1)
          , NODE(lib::Cons, i0, NODE(lib::Cons, i1, NODE(lib::Cons, i0
              , NODE(lib::Cons, i1, NODE(lib::Cons, i2, lib::nil))))
              )
          )
      )
  )

// map (+1) [0,1] -> [1,2]
OPERATION(test_map, "test_map", 0
  , (DT_LEAF
      , REWRITE(
            lib::map
          , NODE(PARTIAL(AddNode,1), i1)
          , NODE(lib::Cons, i0, NODE(lib::Cons,i1,lib::nil))
          )
      )
  )

// concatMap (enumFromTo 0) [0,1,2] -> [0,0,1,0,1,2]
OPERATION(test_concatMap, "test_concatMap", 0
  , (DT_LEAF
      , REWRITE(
            lib::apply
          , NODE(lib::concatMap, NODE(PARTIAL(lib::enumFromTo,1), i0))
          , NODE(lib::Cons, i0, NODE(lib::Cons, i1, NODE(lib::Cons, i2, lib::nil)))
          )
      )
  )


BOOST_AUTO_TEST_CASE(test_list)
{
  BOOST_CHECK(Exec<test_headfail>().fails());
  BOOST_CHECK(Exec<test_tailfail>().fails());
  BOOST_CHECK(Exec<test_enumFrom>().is_list(1,2,3,4));
  BOOST_CHECK(Exec<test_enumFromTo>().is_list(4,5,6,7,8));
  BOOST_CHECK(Exec<test_headtail>().is_int(3));
  BOOST_CHECK(Exec<test_filter>().is_list(1,1));
  BOOST_CHECK(Exec<test_map>().is_list(1,2));
  BOOST_CHECK(Exec<test_concatMap>().is_list(0,0,1,0,1,2));
}

