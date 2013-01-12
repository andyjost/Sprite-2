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
  #define PRELUDE_TP_LIST (lib, ((Nil, "[]", 0))((Cons, ":", 2)))
  TYPE(PRELUDE_TP_LIST)


  // The static constant [].
  STATIC_NODE(nil, Nil);


  // append [] xs = xs
  // append (x:xs) ys = x : (append xs ys)
  OPERATION(AppendNode, "append", 2
    , (DT_BRANCH, RDX[0], PRELUDE_TP_LIST
        , (DT_LEAF, REWRITE(FwdNode, RDX[1]))
        , (DT_LEAF, REWRITE(Cons, IND[0], NODE(AppendNode, IND[1], RDX[1])))
        )
    )

  // head (x:_) = x
  OPERATION(head, "head", 1
    , (DT_BRANCH, RDX[0], PRELUDE_TP_LIST
        , DT_EXEMPT
        , (DT_LEAF, REWRITE (FwdNode, IND[0]))
        )
    )


  // tail (_:xs) = xs
  OPERATION(tail, "tail", 1
    , (DT_BRANCH, RDX[0], PRELUDE_TP_LIST
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
        , COND (GtNode(RDX[0], RDX[1])
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
              LeNode(RDX[0], i0)
            , REWRITE(Nil)
            , REWRITE(takep, RDX[0], RDX[1])
            )
        )
    )

  OPERATION(takep, "takep", 2
    , (DT_BRANCH, RDX[1], PRELUDE_TP_LIST
        , (DT_LEAF, REWRITE(Nil))
        , (DT_LEAF
            , REWRITE(
                  Cons, IND[0]
                , NODE(take, NODE(SubNode, RDX[0], i1), IND[1])
                )
            )
        )
    )


  // THIS IS A PLACEHOLDER --- REPLACE WITH AN ACTUAL APPLY
  OPERATION(xapply, "apply", 2, (DT_LEAF, REWRITE(xapply, RDX[0], RDX[1])))


  // map _ []        = []
  // map f (x:xs)    = f x : map f xs
  OPERATION(map, "map", 2
    , (DT_BRANCH, RDX[1], PRELUDE_TP_LIST
        , (DT_LEAF, REWRITE(Nil))
        , (DT_LEAF
            , REWRITE(
                  Cons
                , NODE(xapply, RDX[0], IND[0])
                , NODE(map,  RDX[0], IND[1])
                )
            )
        )
    )


  // filter _ []       = []
  // filter p (x:xs)   = if p x then x : filter p xs
  //                            else filter p xs
  OPERATION(filter, "filter", 2
    , (DT_BRANCH, RDX[1], PRELUDE_TP_LIST
        , (DT_LEAF, REWRITE(Nil))
        , (DT_LEAF
            , COND(
                  xapply(RDX[0], IND[0])
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
               Cons, RDX[1]
             , NODE(iterate, RDX[0]
             , NODE(xapply, RDX[0], RDX[1]))
             )
         )
     )

  // TODO: comprehension
}}

