#pragma once
#include "sprite/sprite.hpp"
#include "sprite/lib.hpp"

namespace sprite { namespace module { namespace wheel_sieve2
{
  /*
  -- ORIGINAL
    primes :: [Int]
    primes = spiral wheels primes squares
    
    spiral (Wheel s ms ns:ws) ps qs =
      foldr turn0 (roll s) ns
      where
      roll o = foldr (turn o) (foldr (turn o) (roll (o+s)) ns) ms
      turn0  n rs =
        if n<q then n:rs else sp
      turn o n rs =
        let n' = o+n in
        if n'==2 || n'<q then n':rs else dropWhile (<n') sp
      sp = spiral ws (tail ps) (tail qs)
      q = head qs
    
    squares :: [Int]
    squares = [p*p | p <- primes]
    
    data Wheel = Wheel Int [Int] [Int]
    
    wheels :: [Wheel]
    wheels = Wheel 1 [1] [] :
             zipWith3 nextSize wheels primes squares 
    
    nextSize (Wheel s ms ns) p q =
      Wheel (s*p) ms' ns'
      where
      (xs, ns') = span (<=q) (foldr turn0 (roll (p-1) s) ns)
      ms' = foldr turn0 xs ms
      roll 0 _ = []
      roll t o = foldr (turn o) (foldr (turn o) (roll (t-1) (o+s)) ns) ms
      turn0  n rs =
        if n`mod`p>0 then n:rs else rs
      turn o n rs =
        let n' = o+n in
        if n'`mod`p>0 then n':rs else rs

  -- SYNTACTICALLY DESUGARED
    primes :: [Int]
    primes = spiral wheels primes squares
    
    spiral (w@(Wheel s ms ns):ws) ps qs =
      spiral_0 w ps qs (head qs) (spiral ws (tail ps) (tail qs))
    
    spiral_0 (Wheel s ms ns) ps qs q sp = foldr (turn0_0 q sp) (roll_0 q sp s ns ms s) ns
    
    turn_0 q sp o n rs = turn_0_0 q sp rs (o+n)
    turn_0_0 q sp rs n' =
      if n'==2 || n'<q then n':rs else dropWhile (<n') sp
    
    roll_0 q sp s ns ms o =
      foldr (turn_0 q sp o) (foldr (turn_0 q sp o) (roll_0 q sp s ns ms (o+s)) ns) ms
    
    turn0_0 q sp n rs =
      if n<q then n:rs else sp
    
    squares :: [Int]
    squares = map lambda_0 primes
    lambda_0 p = p*p
    
    
    data Wheel = Wheel Int [Int] [Int]
    
    wheels :: [Wheel]
    wheels = Wheel 1 [1] [] : zipWith3 nextSize wheels primes squares
    
    nextSize (Wheel s ms ns) p q =
      nextSize_0 s ms p (span (<=q) (foldr (turn0_1 p) (roll_1 p s ns ms (p-1) s) ns))
    
    nextSize_0 s ms p (xs,ns') = nextSize_1 s p ns' (foldr (turn0_1 p) xs ms)
    
    nextSize_1 s p ns' ms' = Wheel (s*p) ms' ns'
    
    turn0_1 p n rs =
      if n`mod`p>0 then n:rs else rs
    
    roll_1 p s ns ms t o = if t==0 then [] else
        foldr (turn_1 p o) (foldr (turn_1 p o) (roll_1 p s ns ms (t-1) (o+s)) ns) ms
    
    turn_1 p o n rs = turn0_1 p (o+n) rs

  */

  using namespace sprite;
  using lib::constants::i0;
  using lib::constants::i1;
  using lib::constants::i2;


  #define MODULE_12wheel_sieve2_TP_5Wheel           \
      (module::wheel_sieve2, ((Wheel, "Wheel", 3))) \
    /**/
  TYPE(MODULE_12wheel_sieve2_TP_5Wheel)


  struct primes;
  struct spiral;
  struct spiral_0;
  struct turn_0;
  struct turn_0_0;
  struct roll_0;
  struct turn0_0;
  struct squares;
  struct lambda_0;
  struct wheels;
  struct nextSize;
  struct nextSize_0;
  struct nextSize_1;
  struct turn0_1;
  struct roll_1;
  struct turn_1;


  OPERATION(primes, "primes", 0
    , (DT_LEAF
        , REWRITE(spiral, NODE(wheels), NODE(primes), NODE(squares))
        )
    )


  OPERATION(spiral, "spiral", 3
    , (DT_BRANCH, RDX[0], SPRITE_LIB_List
        , DT_EXEMPT
        , (DT_BRANCH, IND[0], MODULE_12wheel_sieve2_TP_5Wheel
            , (DT_LEAF
                , REWRITE(
                      spiral_0
                    , RDX[0]->at(0)
                    , RDX[1]
                    , RDX[2]
                    , NODE(lib::head, RDX[2])
                    , NODE(
                          spiral, RDX[0]->at(1)
                        , NODE(lib::tail, RDX[1])
                        , NODE(lib::tail, RDX[2])
                        )
                    )
                )
            )
        )
    )


  OPERATION(spiral_0, "spiral_0", 5
    , (DT_BRANCH, RDX[0], MODULE_12wheel_sieve2_TP_5Wheel
        , (DT_LEAF
            , REWRITE(
                  lib::foldr
                , NODE(PARTIAL(turn0_0,2), RDX[3], RDX[4])
                , NODE(
                      roll_0, RDX[3], RDX[4]
                    , IND[0], IND[2], IND[1], IND[0]
                    )
                , IND[2]
                )
            )
        )
    )


  OPERATION(turn_0, "turn_0", 5
    , (DT_LEAF
        , REWRITE(
              turn_0_0, RDX[0], RDX[1], RDX[4]
            , NODE(AddNode, RDX[2], RDX[3])
            )
        )
    )


  OPERATION(turn_0_0, "turn_0_0", 4
    , (DT_LEAF
        , IF(
              NODE(
                  lib::BoolOr
                , NODE(EqNode, RDX[3], i2)
                , NODE(LtNode, RDX[3], RDX[0])
                )
            , THEN(lib::Cons, RDX[3], RDX[2])
            , ELSE(lib::dropWhile, NODE(PARTIAL(GeNode,1), RDX[3]), RDX[1])
            )
        )
    )


  OPERATION(roll_0, "roll_0", 6
    , (DT_LEAF
        , REWRITE(
              lib::foldr
            , NODE(PARTIAL(turn_0,3), RDX[0], RDX[1], RDX[5])
            , NODE(
                  lib::foldr
                , NODE(PARTIAL(turn_0,3), RDX[0], RDX[1], RDX[5])
                , NODE(
                      roll_0, RDX[0], RDX[1], RDX[2], RDX[3], RDX[4]
                    , NODE(AddNode, RDX[5], RDX[2])
                    )
                , RDX[3]
                )
            , RDX[4]
            )
        )
    )


  OPERATION(turn0_0, "turn0_0", 4
    , (DT_LEAF
        , IF(
              NODE(LtNode, RDX[2], RDX[0])
            , THEN(lib::Cons, RDX[2], RDX[3])
            , ELSE(FwdNode, RDX[1])
            )
        )
    )


  OPERATION(lambda_0, "lambda_0", 1
    , (DT_LEAF, REWRITE(MulNode, RDX[0], RDX[0]))
    )


  OPERATION(squares, "squares", 0
    , (DT_LEAF, REWRITE(lib::map, NODE(PARTIAL(lambda_0,0)), NODE(primes)))
    )


  OPERATION(wheels, "wheels", 0
    , (DT_LEAF
        , REWRITE(
              lib::Cons
            , NODE(Wheel, i1, NODE(lib::Cons, i1, lib::nil), lib::nil)
            , NODE(
                  lib::zipWith3
                , NODE(PARTIAL(nextSize,0))
                , NODE(wheels)
                , NODE(primes)
                , NODE(squares)
                )
            )
        )
    )


  OPERATION(nextSize, "nextSize", 3
    , (DT_BRANCH, RDX[0], MODULE_12wheel_sieve2_TP_5Wheel
        , (DT_LEAF
            , REWRITE(
                  nextSize_0
                , IND[0], IND[1], RDX[1]
                , NODE(
                      lib::span
                    , NODE(PARTIAL(GtNode,1), RDX[2])
                    , NODE(
                          lib::foldr
                        , NODE(PARTIAL(turn0_1,1), RDX[1])
                        , NODE(
                              roll_1, RDX[1], IND[0], IND[2], IND[1]
                            , NODE(SubNode, RDX[1], i1)
                            , IND[0]
                            )
                        , IND[2]
                        )
                    )
                )
            )
        )
    )


  OPERATION(nextSize_0, "nextSize_0", 4
    , (DT_BRANCH, RDX[3], SPRITE_LIB_Tuple(2)
        , (DT_LEAF
            , REWRITE(
                  nextSize_1, RDX[0], RDX[2], IND[1]
                , NODE(
                      lib::foldr
                    , NODE(PARTIAL(turn0_1,1), RDX[2])
                    , IND[0], RDX[1]
                    )
                )
            )
        )
    )


  OPERATION(nextSize_1, "nextSize_1", 4
    , (DT_LEAF
        , REWRITE(Wheel, NODE(MulNode, RDX[0], RDX[1]), RDX[3], RDX[2])
        )
    )


  OPERATION(turn0_1, "turn0_1", 3
    , (DT_LEAF
        , IF(
              NODE(GtNode, NODE(ModNode, RDX[1], RDX[0]), i0)
            , THEN(lib::Cons, RDX[1], RDX[2])
            , ELSE(FwdNode, RDX[2])
            )
        )
    )


  OPERATION(roll_1, "roll_1", 6
    , (DT_LEAF
        , IF(
              NODE(EqNode, RDX[4], i0)
            , THEN(lib::Nil)
            , ELSE(
                  lib::foldr
                , NODE(PARTIAL(turn_1,2), RDX[0], RDX[5])
                , NODE(
                      lib::foldr
                    , NODE(PARTIAL(turn_1,2), RDX[0], RDX[5])
                    , NODE(
                          roll_1, RDX[0], RDX[1], RDX[2], RDX[3]
                        , NODE(SubNode, RDX[4], i1)
                        , NODE(AddNode, RDX[5], RDX[1])
                        )
                    , RDX[2]
                    )
                , RDX[3]
                )
            )
        )
    )


  OPERATION(turn_1, "turn_1", 4
    , (DT_LEAF
        , REWRITE(turn0_1, RDX[0], NODE(AddNode, RDX[1], RDX[2]), RDX[3])
        )
    )


  OPERATION(MainNode, "main", 0
    , (DT_LEAF, REWRITE(lib::index, NODE(primes), NODE(IntNode, 8000)))
    )


}}}
