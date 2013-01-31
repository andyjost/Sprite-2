#pragma once
#include "sprite/sprite.hpp"
#include "sprite/lib/bool.hpp"
#include "sprite/lib/functional.hpp"
#include "sprite/lib/int.hpp"
#include "sprite/lib/list.hpp"

namespace sprite { namespace module { namespace wheel_sieve1
{
  /*
  -- ORIGINAL
    data Wheel = Wheel Int [Int]
    
    primes :: [Int]
    primes = sieve wheels primes squares
    
    sieve (Wheel s ns:ws) ps qs =
      [n' | o <- s:[s*2,s*3..(head ps-1)*s],
            n <- ns,
            n'<- [n+o], noFactor n'] 
      ++
      sieve ws (tail ps) (tail qs)
      where
      noFactor = if s<=2 then const True else notDivBy ps qs
    
    notDivBy (p:ps) (q:qs) n =
      q > n || n `mod` p > 0 && notDivBy ps qs n
    
    squares :: [Int]
    squares = [p*p | p<-primes]
    
    wheels :: [Wheel]
    wheels = Wheel 1 [1] : zipWith nextSize wheels primes 
    
    nextSize (Wheel s ns) p =
      Wheel (s*p) ns'
      where
      ns' = [n' | o <- [0,s..(p-1)*s],
                  n <- ns,
                  n' <- [n+o], n'`mod`p > 0]

  -- SYNTACTICALLY DESUGARED
    data Wheel = Wheel Int [Int]
    
    primes :: [Int]
    primes = sieve wheels primes squares
    
    sieve ((Wheel s ns):ws) ps qs =
      (gen s ns ps qs) ++ (sieve ws (tail ps) (tail qs))
    noFactor s ps qs = if s<=2 then const True else notDivBy ps qs
    gen s ns ps qs = concatMap (lambda_4 ns (noFactor s ps qs)) (gen_2 s ps)
    gen_2 s ps = s:[s*2,s*3..((head ps)-1)*s]
    lambda_4 ns pred o = concatMap (lambda_5 pred o) ns
    lambda_5 pred o n = filter pred [n+o]
    
    notDivBy (p:ps) (q:qs) n =
      q > n || ((n `mod` p > 0) && (notDivBy ps qs n))
    
    squares :: [Int]
    squares = map lambda_0 primes
    lambda_0 p = p*p
    
    wheels :: [Wheel]
    wheels = (Wheel 1 [1]) : zipWith nextSize wheels primes
    
    nextSize (Wheel s ns) p = Wheel (s*p) (ns' s ns p)
    ns' s ns p = concatMap (lambda_1 ns p) [0,s..(p-1)*s]
    lambda_1 ns p o = concatMap (lambda_2 p o) ns
    lambda_2 p o n = filter (lambda_3 p) [n+o]
    lambda_3 p n' = n'`mod`p > 0

  */

  using namespace sprite;
  using lib::constants::i0;
  using lib::constants::i1;
  using lib::constants::i2;
  using lib::constants::i3;


  #define MODULE_12wheel_sieve1_TP_5Wheel           \
      (module::wheel_sieve1, ((Wheel, "Wheel", 2))) \
    /**/
  TYPE(MODULE_12wheel_sieve1_TP_5Wheel)


  struct gen;
  struct gen_2;
  struct lambda_0;
  struct lambda_1;
  struct lambda_2;
  struct lambda_3;
  struct lambda_4;
  struct lambda_5;
  struct nextSize;
  struct notDivBy;
  struct ns_;
  struct sieve;
  struct squares;
  struct wheels;

  OPERATION(primes, "primes", 0
    , (DT_LEAF
        , REWRITE(sieve, NODE(wheels), NODE(primes), NODE(squares))
        )
    )


  OPERATION(sieve, "sieve", 3
    , (DT_BRANCH, RDX[0], SPRITE_LIB_List
        , DT_EXEMPT
        , (DT_BRANCH, IND[0], MODULE_12wheel_sieve1_TP_5Wheel
            , (DT_LEAF
                , REWRITE(
                      lib::append
                    , NODE(gen, IND[0], IND[1], RDX[1], RDX[2])
                    , NODE(sieve, RDX[0]->at(1)
                        , NODE(lib::tail, RDX[1])
                        , NODE(lib::tail, RDX[2])
                        )
                    )
                )
            )
        )
    )


  OPERATION(noFactor, "noFactor", 3
    , (DT_LEAF
        , IF(
              NODE(LeNode, RDX[0], i2)
            , THEN(PARTIAL(lib::const_,1), lib::true_)
            , ELSE(PARTIAL(notDivBy,2), RDX[1], RDX[2])
            )
        )
    )


  OPERATION(gen, "gen", 4
    , (DT_LEAF
        , REWRITE(
              lib::apply
            , NODE(
                  lib::concatMap
                , NODE(
                      PARTIAL(lambda_4,2)
                    , RDX[1]
                    , NODE(noFactor, RDX[0], RDX[2], RDX[3])
                    )
                )
            , NODE(gen_2, RDX[0], RDX[2])
            )
        )
    )


  OPERATION(gen_2, "gen_2", 2
    , (DT_LEAF
        , REWRITE(
              lib::Cons
            , RDX[0]
            , NODE(
                  lib::enumFromThenTo
                , NODE(MulNode, RDX[0], i2)
                , NODE(MulNode, RDX[0], i3)
                , NODE(
                      MulNode
                    , NODE(SubNode, NODE(lib::head, RDX[1]), i1)
                    , RDX[0]
                    )
                )
            )
        )
    )


  OPERATION(lambda_0, "lambda_0", 1
    , (DT_LEAF, REWRITE(MulNode, RDX[0], RDX[0]))
    )


  OPERATION(lambda_1, "lambda_1", 3
    , (DT_LEAF
        , REWRITE(
              lib::apply
            , NODE(
                  lib::concatMap
                , NODE(PARTIAL(lambda_2,2), RDX[1], RDX[2])
                )
            , RDX[0]
            )
        )
    )


  OPERATION(lambda_2, "lambda_2", 3
    , (DT_LEAF
        , REWRITE(lib::filter
            , NODE(PARTIAL(lambda_3,1), RDX[0])
            , NODE(lib::Cons, NODE(AddNode, RDX[2], RDX[1]), lib::nil)
            )
        )
    )


  OPERATION(lambda_3, "lambda_3", 2
    , (DT_LEAF
        , REWRITE(
              GtNode
            , NODE(ModNode, RDX[1], RDX[0])
            , i0
            )
        )
    )


  OPERATION(lambda_4, "lambda_4", 3
    , (DT_LEAF
        , REWRITE(
              lib::apply
            , NODE(
                  lib::concatMap
                , NODE(PARTIAL(lambda_5,2), RDX[1], RDX[2])
                )
            , RDX[0]
            )
        )
    )


  OPERATION(lambda_5, "lambda_5", 3
    , (DT_LEAF
        , REWRITE(
              lib::filter
            , RDX[0]
            , NODE(lib::Cons, NODE(AddNode, RDX[2], RDX[1]), lib::nil)
            )
        )
    )


  OPERATION(notDivBy, "notDivBy", 3
    , (DT_BRANCH, RDX[0], SPRITE_LIB_List
        , DT_EXEMPT
        , (DT_BRANCH, RDX[1], SPRITE_LIB_List
            , DT_EXEMPT
            , (DT_LEAF
                , REWRITE(
                      lib::BoolOr
                    , NODE(GtNode, IND[0], RDX[2])
                    , NODE(
                          lib::BoolAnd
                        , NODE(GtNode, NODE(ModNode, RDX[2], RDX[0]->at(0)), i0)
                        , NODE(notDivBy, RDX[0]->at(1), IND[1], RDX[2])
                        )
                    )
                )
            )
        )
    )


  OPERATION(squares, "squares", 0
    , (DT_LEAF, REWRITE(lib::map, NODE(PARTIAL(lambda_0,0)), NODE(primes)))
    )


  OPERATION(wheels, "wheels", 0
    , (DT_LEAF
        , REWRITE(
              lib::Cons
            , NODE(Wheel, i1, NODE(lib::Cons, i1, lib::nil))
            , NODE(
                  lib::zipWith
                , NODE(PARTIAL(nextSize,0))
                , NODE(wheels)
                , NODE(primes)
                )
            )
        )
    )


  OPERATION(nextSize, "nextSize", 2
    , (DT_BRANCH, RDX[0], MODULE_12wheel_sieve1_TP_5Wheel
        , (DT_LEAF
            , REWRITE(
                  Wheel
                , NODE(MulNode, IND[0], RDX[1])
                , NODE(ns_, IND[0], IND[1], RDX[1])
                )
            )
        )
    )


  OPERATION(ns_, "ns'", 3
    , (DT_LEAF
        , REWRITE(
              lib::apply
            , NODE(
                  lib::concatMap
                , NODE(PARTIAL(lambda_1,2), RDX[1], RDX[2])
                )
            , NODE(
                  lib::enumFromThenTo
                , i0
                , RDX[0]
                , NODE(MulNode, NODE(SubNode, RDX[2], i1), RDX[0])
                )
            )
        )
    )


  OPERATION(MainNode, "main", 0
    , (DT_LEAF, REWRITE(lib::index, NODE(primes), NODE(IntNode, 20000)))
    )
}}}
