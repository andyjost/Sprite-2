/**
 * @file Provides the list library.
 */

#pragma once
#include "sprite/lib/functional.hpp"
#include "sprite/lib/int.hpp"
#include "sprite/lib/tuple.hpp"

namespace sprite { namespace lib
{
  using namespace constants; // for i0, i1, ...

  // The List type.
  #define SPRITE_LIB_List (lib, ((Nil, "[]", 0))((Cons, ":", 2)))
  TYPE(SPRITE_LIB_List)


  // The static constant [].
  STATIC_NODE(nil, Nil);


  // length []     = 0
  // length (_:xs) = 1 + length xs
  OPERATION(length, "length", 1
    , (DT_BRANCH, RDX[0], SPRITE_LIB_List
        , (DT_LEAF, REWRITE(IntNode, 0L))
        , (DT_LEAF, REWRITE(AddNode, i1, NODE(length, IND[1])))
        )
    )


  // [] ++ ys = ys
  // (x:xs) ++ ys = x : xs++ys
  OPERATION(append, "++", 2
    , (DT_BRANCH, RDX[0], SPRITE_LIB_List
        , (DT_LEAF, REWRITE(FwdNode, RDX[1]))
        , (DT_LEAF, REWRITE(Cons, IND[0], NODE(append, IND[1], RDX[1])))
        )
    )

  // head (x:_) = x
  OPERATION(head, "head", 1
    , (DT_BRANCH, RDX[0], SPRITE_LIB_List
        , DT_EXEMPT
        , (DT_LEAF, REWRITE (FwdNode, IND[0]))
        )
    )


  // tail (_:xs) = xs
  OPERATION(tail, "tail", 1
    , (DT_BRANCH, RDX[0], SPRITE_LIB_List
        , DT_EXEMPT
        , (DT_LEAF, REWRITE (FwdNode, IND[1]))
        )
    )


  // enumFrom n = n : enumFrom (n+1)
  // denoted as [n..]
  OPERATION(enumFrom, "enumFrom", 1
    , (DT_LEAF
        , REWRITE(Cons, RDX[0]
            , NODE (enumFrom, NODE(AddNode, RDX[0], i1))
            )
        )
    )


  // enumFromTo n m = if n>m then [] else n : enumFromTo (n+1) m
  // denoted as [n..m]
  OPERATION(enumFromTo, "enumFromTo", 2
    , (DT_LEAF
        , IF(
              NODE(GtNode, RDX[0], RDX[1])
            , THEN(Nil)
            , ELSE(
                  Cons
                , RDX[0]
                , NODE(enumFromTo, NODE(AddNode, RDX[0], i1), RDX[1])
                )
            )
        )
    )

  // enumFromThenTo n m z = enumFromDeltaTo n (m-n) z
  // enumFromDeltaTo n d z = if n>z then [] else n : enumFromDeltaTo (n+d) d z
  // denoted as [n,m..z]

  struct enumFromDeltaTo;

  OPERATION(enumFromThenTo, "enumFromThenTo", 3
    , (DT_LEAF
        , REWRITE(
              enumFromDeltaTo, RDX[0], NODE(SubNode,RDX[1],RDX[0]), RDX[2]
            )
        )
    )

  OPERATION(enumFromDeltaTo, "enumFromDeltaTo", 3
    , (DT_LEAF
        , IF(
              NODE(GtNode, RDX[0], RDX[2])
            , THEN(Nil)
            , ELSE(
                  Cons
                , RDX[0]
                , NODE(
                      enumFromDeltaTo
                    , NODE(AddNode, RDX[0], RDX[1])
                    , RDX[1]
                    , RDX[2]
                    )
                )
            )
        )
    )


  // take n l = if n<=0 then [] else takep n l
  //    where takep _ []     = []
  //          takep m (x:xs) = x : take (m-1) xs

  struct takep;

  OPERATION(take, "take", 2
    , (DT_LEAF 
        , IF(
              NODE(LeNode, RDX[0], i0)
            , THEN(Nil)
            , ELSE(takep, RDX[0], RDX[1])
            )
        )
    )

  OPERATION(takep, "takep", 2
    , (DT_BRANCH, RDX[1], SPRITE_LIB_List
        , (DT_LEAF, REWRITE(Nil))
        , (DT_LEAF
            , REWRITE(
                  Cons, IND[0]
                , NODE(take, NODE(SubNode, RDX[0], i1), IND[1])
                )
            )
        )
    )


  // map _ []        = []
  // map f (x:xs)    = f x : map f xs
  OPERATION(map, "map", 2
    , (DT_BRANCH, RDX[1], SPRITE_LIB_List
        , (DT_LEAF, REWRITE(Nil))
        , (DT_LEAF
            , REWRITE(
                  Cons
                , NODE(apply, RDX[0], IND[0])
                , NODE(map,  RDX[0], IND[1])
                )
            )
        )
    )
  

  // filter _ []       = []
  // filter p (x:xs)   = if p x then x : filter p xs
  //                            else filter p xs
  OPERATION(filter, "filter", 2
    , (DT_BRANCH, RDX[1], SPRITE_LIB_List
        , (DT_LEAF, REWRITE(Nil))
        , (DT_LEAF
            , IF(
                  NODE(apply, RDX[0], IND[0])
                , THEN(Cons, IND[0], NODE(filter, RDX[0], IND[1]))
                , ELSE(filter, RDX[0], IND[1])
                )
            )
        )
    )


  // iterate f x = x : iterate f (f x)
  OPERATION(iterate, "iterate", 2
    , (DT_LEAF
        , REWRITE(
              Cons, RDX[1], NODE(iterate, RDX[0], NODE(apply,RDX[0],RDX[1]))
            )
        )
    )

  // index xs i = if i < 0 then fail else indexp xs i
  //    where indexp (x::xs) i = if i == 0 then x else indexp xs (i-1)
  // denoted as a !! i
  struct indexp;

  OPERATION(index, "index", 2
    , (DT_LEAF
        , IF(
              NODE(LtNode, RDX[1], i0)
            , THEN(FailNode)
            , ELSE(indexp, RDX[0], RDX[1])
            )
        )
    )

  OPERATION(indexp, "indexp", 2
    , (DT_BRANCH, RDX[0], SPRITE_LIB_List
        , DT_EXEMPT
        , (DT_LEAF
            , IF(
                  NODE(EqNode, RDX[1], i0)
                , THEN(FwdNode, IND[0])
                , ELSE(indexp, IND[1], NODE(SubNode, RDX[1], i1))
                )
            )
        )
    )


  // foldr            :: (a->b->b) -> b -> [a] -> b
  // foldr _ z []     = z
  // foldr f z (x:xs) = f x (foldr f z xs)
  OPERATION(foldr, "foldr", 3
    , (DT_BRANCH, RDX[2], SPRITE_LIB_List
        , (DT_LEAF, REWRITE(FwdNode, RDX[1]))
        , (DT_LEAF
            , REWRITE(
                  apply
                , NODE(apply, RDX[0], IND[0])
                , NODE(foldr, RDX[0], RDX[1], IND[1])
                )
            )
        )
    )


  // concat            :: [[a]] -> [a]
  // concat l          = foldr (++) [] l
  OPERATION(concat, "concat", 1
    , (DT_LEAF, REWRITE(foldr, NODE(PARTIAL(append,0)), nil, RDX[0]))
    )
  

  // concatMap         :: (a -> [b]) -> [a] -> [b]
  // concatMap f       = concat . map f
  OPERATION(concatMap, "concatMap", 1
    , (DT_LEAF
        , REWRITE(
              compose
            , NODE(PARTIAL(concat,0))
            , NODE(PARTIAL(map,1), RDX[0])
            )
        )
    )


  // zipWith :: (a->b->c) -> [a]->[b]->[c]
  // zipWith f (a:as) (b:bs) = f a b : zipWith f as bs
  // zipWith _ _      _      = []
  OPERATION(zipWith, "zipWith", 3
    , (DT_BRANCH, RDX[1], SPRITE_LIB_List
        , (DT_LEAF, REWRITE(lib::Nil))
        , (DT_BRANCH, RDX[2], SPRITE_LIB_List
            , (DT_LEAF, REWRITE(lib::Nil))
            , (DT_LEAF
                , REWRITE(
                      lib::Cons
                    , NODE(
                          lib::apply
                        , NODE(lib::apply, RDX[0], RDX[1]->at(0))
                        , IND[0]
                        )
                    , NODE(zipWith, RDX[0], RDX[1]->at(1), IND[1])
                    )
                )
            )
        )
    )


  // zipWith3                :: (a->b->c->d) -> [a]->[b]->[c]->[d]
  // zipWith3 z (a:as) (b:bs) (c:cs)
  //                         =  z a b c : zipWith3 z as bs cs
  // zipWith3 _ _ _ _        =  []
  OPERATION(zipWith3, "zipWith3", 4
    , (DT_BRANCH, RDX[1], SPRITE_LIB_List
        , (DT_LEAF, REWRITE(lib::Nil))
        , (DT_BRANCH, RDX[2], SPRITE_LIB_List
            , (DT_LEAF, REWRITE(lib::Nil))
            , (DT_BRANCH, RDX[3], SPRITE_LIB_List
                , (DT_LEAF, REWRITE(lib::Nil))
                , (DT_LEAF
                    , REWRITE(
                          Cons
                        , NODE(
                              lib::apply3
                            , RDX[0], RDX[1]->at(0), RDX[2]->at(0), IND[0]
                            )
                        , NODE(
                              zipWith3
                            , RDX[0], RDX[1]->at(1), RDX[2]->at(1), IND[1]
                            )
                        )
                    )
                )
            )
        )
    )

  // --- Test for zipWith3
  // using namespace lib::constants;
  // OPERATION(add3, "add3", 3
  //   , (DT_LEAF, REWRITE(AddNode, NODE(AddNode, RDX[0], RDX[1]), RDX[2]))
  //   )

  // OPERATION(MainNode, "main", 0
  //   , (DT_LEAF
  //       , REWRITE(
  //             lib::zipWith3
  //           , NODE(PARTIAL(add3,0))
  //           , NODE(lib::Cons, i1, NODE(lib::Cons, i2, lib::nil))
  //           , NODE(lib::Cons, i4, NODE(lib::Cons, i5, lib::nil))
  //           , NODE(lib::Cons, i7, NODE(lib::Cons, i8, lib::nil))
  //           )
  //       )
  //   )


  // span                    :: (a -> Bool) -> [a] -> ([a],[a])
  // span _ xs@[]            =  (xs, xs)
  // span p xs@(x:xs')
  //          | p x          =  let (ys,zs) = span p xs' in (x:ys,zs)
  //          | otherwise    =  ([],xs)
  // -- DESUGARED
  // span _ xs@[]            =  (xs, xs)
  // span p xs@(x:xs') = if p x then auxspan (span p xs') x else ([],xs)
  // auxspan (ys,zs) x = (x:ys,zs)

  struct auxspan;
  OPERATION(span, "span", 2
    , (DT_BRANCH, RDX[1], SPRITE_LIB_List
        , (DT_LEAF, REWRITE(Tuple2, RDX[1], RDX[1]))
        , (DT_LEAF
            , IF(
                  NODE(apply, RDX[0], IND[0])
                , THEN(auxspan, NODE(span, RDX[0], IND[1]), IND[0])
                , ELSE(Tuple2, nil, RDX[1])
                )
            )
        )
    )

  OPERATION(auxspan, "auxspan", 2
    , (DT_BRANCH, RDX[0], SPRITE_LIB_Tuple(2)
        , (DT_LEAF, REWRITE(Tuple2, NODE(Cons, RDX[1], IND[0]), IND[1]))
        )
    )


  // dropWhile               :: (a -> Bool) -> [a] -> [a]
  // dropWhile _ []          =  []
  // dropWhile p xs@(x:xs')
  //             | p x       =  dropWhile p xs'
  //             | otherwise =  xs
  // -- DESUGARED
  // dropWhile _ []          =  []
  // dropWhile p xs@(x:xs') = if p x then dropWhile p xs' else xs

  OPERATION(dropWhile, "dropWhile", 2
    , (DT_BRANCH, RDX[1], SPRITE_LIB_List
        , (DT_LEAF, REWRITE(Nil))
        , (DT_LEAF
            , IF(
                  NODE(apply, RDX[0], IND[0])
                , THEN(dropWhile, RDX[0], IND[1])
                , ELSE(FwdNode, RDX[1])
                )
            )
        )
    )
}}

