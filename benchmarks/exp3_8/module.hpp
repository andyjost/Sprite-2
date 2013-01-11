#pragma once
#include "sprite/sprite.hpp"

namespace exp3_8
{
  /*
    data Nat = Z | S Nat deriving (Eq,Ord, Show {-was:Text-})

    -- First arg is a Nat, second is an Int.
    myadd Z y = y
    myadd (S x) y = S (myadd x y)

    -- First arg is an Int, second is a Nat.
    mymul x Z = Z
    mymul x (S y) = myadd (mymul x y) x
    fromInteger x = if x < 1 then Z else S (fromInteger (x-1))

    myint Z = 0
    myint (S x) = 1 + myint x

    mypow x Z = S Z
    mypow x (S y) = mymul x (mypow x y)

    main = myint (mypow (fromInteger 3) (fromInteger 8))
  */

  using namespace sprite;

  #define TP_Nat ((ZeroNode, "O", 0))((SuccNode, "S", 1))
  TYPE(TP_Nat)

  STATIC_NODE(one, IntNode, 1);
  STATIC_NODE(n_zero, ZeroNode);

  OPERATION(MyAddNode, "myadd", 2
    , (DT_BRANCH, RDX[0], TP_Nat
        , (DT_LEAF, REWRITE(FwdNode, RDX[1]))
        , (DT_LEAF, REWRITE(SuccNode, NODE(MyAddNode, IND[0], RDX[1])))
        )
    )

  OPERATION(MyMulNode, "mymul", 2
    , (DT_BRANCH, RDX[1], TP_Nat
        , (DT_LEAF, REWRITE(ZeroNode))
        , (DT_LEAF, REWRITE(MyAddNode, NODE(MyMulNode, RDX[0], IND[0]), RDX[0]))
        )
    )

  OPERATION(FromIntegerNode, "fromInteger", 1
    , (DT_LEAF
        , COND(
              LtNode(RDX[0], one)
            , REWRITE(ZeroNode)
            , REWRITE(
                  SuccNode
                , NODE(FromIntegerNode, NODE(SubNode, RDX[0], one))
                )
            )
        )
    )

  OPERATION(MyIntNode, "myint", 1
    , (DT_BRANCH, RDX[0], TP_Nat
        , (DT_LEAF, REWRITE(IntNode, 0L))
        , (DT_LEAF, REWRITE(AddNode, one, NODE(MyIntNode, IND[0])))
        )
    )

  OPERATION(MyPowNode, "mypow", 2
    , (DT_BRANCH, RDX[1], TP_Nat
        , (DT_LEAF, REWRITE(SuccNode, n_zero))
        , (DT_LEAF, REWRITE(MyMulNode, RDX[0], NODE(MyPowNode, RDX[0], IND[0])))
        )
    )

  OPERATION(MainNode, "main", 0
    , (DT_LEAF
        , REWRITE(
              MyIntNode
            , NODE(
                  MyPowNode
                , NODE(FromIntegerNode, NODE(IntNode, 3))
                , NODE(FromIntegerNode, NODE(IntNode, 8))
                )
            )
        )
    )
}
