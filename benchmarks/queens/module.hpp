#pragma once
#include "sprite/sprite.hpp"
#include "sprite/lib/int.hpp"
#include "sprite/lib/list.hpp"
#include "sprite/lib/bool.hpp"

namespace sprite { namespace module { namespace queens
{
  /*
  -- ORIGINAL

  nsoln nq = length (gen nq)
      where
      safe :: Int -> Int -> [Int] -> Bool
      safe x d []    = True
      safe x d (q:l) = x /= q && x /= q+d && x /= q-d && safe x (d+1) l
    
      gen :: Int -> [[Int]]
      gen 0 = [[]]
      gen n = [ (q:b) | b <- gen (n-1), q <- [1..nq], safe q 1 b]

  -- SYNTACTICALLY DESUGARED

  nsoln nq = length (gen nq nq)
  gen r l = if l==0 then [[]]
     else filter lambda_0 (concatMap (lambda_1 r) (gen r (l-1)))

  lambda_0 (q:b) = safe q 1 b
  lambda_1 r b = (map (lambda_2 b) [1..r])
  lambda_2 b q = q:b

  safe _ _ []    = True
  safe x d (q:l) = x /= q && x /= q+d && x /= q-d && safe x (d+1) l

  */

  using namespace sprite;
  using lib::constants::i0;
  using lib::constants::i1;
  using lib::constants::i10;
  using lib::constants::i12;

  // safe _ _ []    = True
  // safe x d (q:l) = x /= q && x /= q+d && x /= q-d && safe x (d+1) l
  OPERATION(safe, "safe", 3
    , (DT_BRANCH, RDX[2], SPRITE_LIB_List
        , (DT_LEAF, REWRITE(BoolNode, true))
        , (DT_LEAF
            , REWRITE(lib::BoolAnd
               , NODE(NeNode, RDX[0], IND[0])
               , NODE(lib::BoolAnd
                   , NODE(NeNode, RDX[0], NODE(AddNode, IND[0], RDX[1]))
                   , NODE(lib::BoolAnd
                       , NODE(NeNode, RDX[0], NODE(SubNode, IND[0], RDX[1]))
                       , NODE(safe, RDX[0], NODE(AddNode, RDX[1], i1), IND[1])
                       )
                   )
               )
            )
        )
    )


  // lambda_2 b q = q:b
  OPERATION(lambda_2, "lambda_2", 2
    , (DT_LEAF, REWRITE(lib::Cons, RDX[1], RDX[0]))
    )


  // lambda_1 r b = (map (lambda_2 b) [1..r])
  OPERATION(lambda_1, "lambda_1", 2
    , (DT_LEAF
        , REWRITE(lib::map
            , NODE(PARTIAL(lambda_2,1), RDX[1])
            , NODE(lib::enumFromTo, i1, RDX[0])
            )
        )
    )


  // lambda_0 (q:b) = safe q 1 b
  OPERATION(lambda_0, "lambda_0", 1
    , (DT_BRANCH, RDX[0], SPRITE_LIB_List
       , DT_EXEMPT
       , (DT_LEAF, REWRITE(safe, IND[0], i1, IND[1]))
       )
    )


  // gen r l = if l==0 then [[]]
  // else filter lambda_0 (concatMap (lambda_1 r) (gen r (l-1)))
  OPERATION(gen, "gen", 2
    , (DT_LEAF
        , IF(
              NODE(EqNode, RDX[1], i0)
            , THEN(lib::Cons, lib::nil, lib::nil)
            , ELSE(
                  lib::filter
                , NODE(PARTIAL(lambda_0,0))
                , NODE(lib::apply
                    , NODE(lib::concatMap, NODE(PARTIAL(lambda_1,1), RDX[0]))
                    , NODE(gen, RDX[0], NODE(SubNode, RDX[1], i1))
                    )
                )
            )
        )
    )


  OPERATION(nsoln, "nsoln", 1
    , (DT_LEAF, REWRITE(lib::length, NODE(gen, RDX[0], RDX[0])))
    )


  OPERATION(MainNode, "main", 0, (DT_LEAF, REWRITE(nsoln, i12)))
}}}
