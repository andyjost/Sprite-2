#pragma once
#include "sprite/sprite.hpp"
#include "sprite/lib/list.hpp"
#include "sprite/lib/int.hpp"

namespace sprite { namespace module { namespace primes
{
  /*
    suCC :: Int -> Int
    suCC x = x + 1
    
    isdivs :: Int  -> Int -> Bool
    isdivs n x = mod x n /= 0
    
    the_filter :: [Int] -> [Int]
    the_filter (n:ns) = filter (isdivs n) ns
    
    primes :: [Int]
    primes = map head (iterate the_filter (iterate suCC 2))

    main = do
      print $ primes !! 5500
  */

  using namespace sprite;
  using namespace lib::constants;

  OPERATION(suCC, "suCC", 1, (DT_LEAF, REWRITE(AddNode, RDX[0], i1)))


  OPERATION(isdivs, "isdivs", 2
    , (DT_LEAF, REWRITE(NeNode, NODE(ModNode, RDX[1], RDX[0]), i0))
    )


  OPERATION(the_filter, "the_filter", 1
    , (DT_BRANCH, RDX[0], SPRITE_LIB_List
        , DT_EXEMPT
        , (DT_LEAF, REWRITE(lib::filter, NODE(PARTIAL(isdivs,1), IND[0]), IND[1]))
        )
    )


  namespace constants
  {
    STATIC_NODE(the_filter, PARTIAL(primes::the_filter,0));
    STATIC_NODE(head, PARTIAL(lib::head,0));
  }


  OPERATION(primes, "primes", 0
    , (DT_LEAF
        , REWRITE(
              lib::map
            , constants::head
            , NODE(
                  lib::iterate
                , constants::the_filter
                , NODE(lib::iterate, NODE(PARTIAL(suCC,0)), i2)
                )
            )
        )
    )


  OPERATION(MainNode, "main", 0
    , (DT_LEAF, REWRITE(lib::index, NODE(primes), NODE(IntNode, 5500)))
    )
}}}
