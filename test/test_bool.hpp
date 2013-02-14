#pragma once
#include "test.hpp"
using namespace sprite::lib::constants;

// [ True & True, True & False, False & True, False & False ]
//     -> [ True, False, False, False ]
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

// [ True | True, True | False, False | True, False | False ]
//     -> [ True, True, True, False ]
OPERATION(test_BoolOr, "test_BoolOr", 0
  , (DT_LEAF
      , REWRITE(
            lib::Cons
          , NODE(lib::BoolOr, lib::true_, lib::true_)
          , NODE(
                lib::Cons
              , NODE(lib::BoolOr, lib::true_, lib::false_)
              , NODE(
                    lib::Cons
                  , NODE(lib::BoolOr, lib::false_, lib::true_)
                  , NODE(
                        lib::Cons
                      , NODE(lib::BoolOr, lib::false_, lib::false_)
                      , lib::nil
                      )
                  )
              )
          )
      )
  )

// [ not True, not False ] -> [ False, True ]
OPERATION(test_BoolNot, "test_BoolNot", 0
  , (DT_LEAF
      , REWRITE(
            lib::Cons
          , NODE(lib::BoolNot, lib::true_)
          , NODE(
                lib::Cons
              , NODE(lib::BoolNot, lib::false_)
              , lib::nil
              )
          )
      )
  )

BOOST_AUTO_TEST_CASE(test_bool)
{
  SystemInitializer sys;
  BOOST_CHECK(Exec<test_BoolAnd>().is_list(true, false, false, false));
  BOOST_CHECK(Exec<test_BoolOr>().is_list(true, true, true, false));
  BOOST_CHECK(Exec<test_BoolNot>().is_list(false, true));
}
