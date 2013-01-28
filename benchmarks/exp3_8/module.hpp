#pragma once
#include "sprite/sprite.hpp"
#include "sprite/lib/int.hpp"

namespace sprite { namespace module { namespace exp3_8
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
  using lib::constants::i1;

  #define MODULE_6exp3_8_TP_3Nat                         \
      (module::exp3_8, ((Zero, "O", 0))((Succ, "S", 1))) \
    /**/
  TYPE(MODULE_6exp3_8_TP_3Nat)

  STATIC_NODE(zero, Zero);

  OPERATION(MyAddNode, "myadd", 2
    , (DT_BRANCH, RDX[0], MODULE_6exp3_8_TP_3Nat
        , (DT_LEAF, REWRITE(FwdNode, RDX[1]))
        , (DT_LEAF, REWRITE(Succ, NODE(MyAddNode, IND[0], RDX[1])))
        )
    )

  OPERATION(MyMulNode, "mymul", 2
    , (DT_BRANCH, RDX[1], MODULE_6exp3_8_TP_3Nat
        , (DT_LEAF, REWRITE(Zero))
        , (DT_LEAF, REWRITE(MyAddNode, NODE(MyMulNode, RDX[0], IND[0]), RDX[0]))
        )
    )

  OPERATION(FromIntegerNode, "fromInteger", 1
    , (DT_LEAF
        , IF(
              NODE(LtNode, RDX[0], i1)
            , THEN(Zero)
            , ELSE(Succ, NODE(FromIntegerNode, NODE(SubNode, RDX[0], i1)))
            )
        )
    )

  OPERATION(MyIntNode, "myint", 1
    , (DT_BRANCH, RDX[0], MODULE_6exp3_8_TP_3Nat
        , (DT_LEAF, REWRITE(IntNode, 0L))
        , (DT_LEAF, REWRITE(AddNode, i1, NODE(MyIntNode, IND[0])))
        )
    )

  OPERATION(MyPowNode, "mypow", 2
    , (DT_BRANCH, RDX[1], MODULE_6exp3_8_TP_3Nat
        , (DT_LEAF, REWRITE(Succ, zero))
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
}}}
