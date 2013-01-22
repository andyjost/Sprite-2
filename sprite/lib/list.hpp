/**
 * @file Provides the list library.
 */

#pragma once
#include "sprite/sprite.hpp"
#include "sprite/lib/int.hpp"

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
        , COND(
              NODE(GtNode, RDX[0], RDX[1])
            , REWRITE(Nil)
            , REWRITE(Cons, RDX[0]
                , NODE(enumFromTo, NODE(AddNode, RDX[0], i1), RDX[1])
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
        , COND(
              NODE(LeNode, RDX[0], i0)
            , REWRITE(Nil)
            , REWRITE(takep, RDX[0], RDX[1])
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
                , APPLY(RDX[0], IND[0])
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
            , COND(
                  APPLY(RDX[0], IND[0])
                , REWRITE(Cons, IND[0], NODE(filter, RDX[0], IND[1]))
                , REWRITE(filter, RDX[0], IND[1])
                )
            )
        )
    )


  // iterate f x = x : iterate f (f x)
  OPERATION(iterate, "iterate", 2
    , (DT_LEAF
        , REWRITE(
              Cons, RDX[1], NODE(iterate, RDX[0], APPLY(RDX[0],RDX[1]))
            )
        )
    )

  // index xs i = if i < 0 then fail else indexp xs i
  //    where indexp (x::xs) i = if i == 0 then x else indexp xs (i-1)
  // denoted as a !! i
  struct indexp;

  OPERATION(index, "index", 2
    , (DT_LEAF
        , COND(
              NODE(LtNode, RDX[1], i0)
            , REWRITE(FailNode)
            , REWRITE(indexp, RDX[0], RDX[1])
            )
        )
    )

  OPERATION(indexp, "indexp", 2
    , (DT_BRANCH, RDX[0], SPRITE_LIB_List
        , DT_EXEMPT
        , (DT_LEAF
            , COND(
                  NODE(EqNode, RDX[1], i0)
                , REWRITE(FwdNode, IND[0])
                , REWRITE(indexp, IND[1], NODE(SubNode, RDX[1], i1))
                )
            )
        )
    )


  // (.)   :: (b -> c) -> (a -> b) -> (a -> c)
  // f . g = \x -> f (g x)
  struct auxcomp;

  // ??? FwdNode is a workaround !!!
  OPERATION(compose, ".", 2
    , (DT_LEAF, REWRITE(FwdNode, NODE(PARTIAL(auxcomp,2), RDX[0], RDX[1])))
    )

  // auxcomp f g x = f(g(x))
  // ??? FwdNode is a workaround !!!
  OPERATION(auxcomp, "auxcomp", 3
    , (DT_LEAF, REWRITE(FwdNode, APPLY(RDX[0], APPLY(RDX[1],RDX[2]))))
    )


  // foldr            :: (a->b->b) -> b -> [a] -> b
  // foldr _ z []     = z
  // foldr f z (x:xs) = f x (foldr f z xs)
  OPERATION(foldr, "foldr", 3
    , (DT_BRANCH, RDX[2], SPRITE_LIB_List
        , (DT_LEAF, REWRITE(FwdNode, RDX[1]))
        // ??? FwdNode is a workaround !!!
        , (DT_LEAF
            , REWRITE(
                  FwdNode
                , APPLY(
                      APPLY(RDX[0],IND[0]), NODE(foldr, RDX[0], RDX[1], IND[1])
                    )
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

  // TODO: comprehension
}}

