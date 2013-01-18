// Note: this fails because the N rule must be applied repeatedly until the
// result is a value.

#pragma once
#include "sprite/sprite.hpp"
#include "sprite/lib/list.hpp"

namespace sprite { namespace module { namespace reverse
{
  /*
  -- Haskell benchmark: naive reverse on buil-in lists
  
  data Nat = O | S Nat
  
  add O n = n
  add (S x) y = S (add x y)
  
  double x = add x x
  
  mult O _ = O
  mult (S x) y = add y (mult x y)
  
  two = S (S O)
  four = double two
  nat16 = mult four four
  nat256 = mult nat16 nat16
  nat4096 = mult nat256 nat16
  nat16384 = mult nat4096 four
  nat256k = mult nat16384 nat16
  nat1M = mult nat256k four
  nat16M = mult nat1M nat16
  nat256M = mult nat16M nat16
  nat1G = mult nat256M four
  
  data MyBool = MyTrue | MyFalse
  
  append [] xs = xs
  append (x:xs) ys = x : (append xs ys)
  
  rev [] = []
  rev (x:xs) = append (rev xs) [x]
  
  natList O = []
  natList (S x) = (S x) : (natList x)
  
  isList [] = MyTrue
  isList (_:xs) = isList xs
  
  goal1 = rev (natList nat16)
  goal2 = rev (natList nat256)
  goal3 = isList (rev (natList nat4096))
  goal4 = isList (rev (natList nat16384))
  goal5 = isList (natList nat16384)
  goal6 = isList (natList nat16)
  goal7 = isList (natList two)
  goal8 = isList (natList nat256k)
  goal9 = isList (natList nat1M)
  goal10 = isList (natList nat16M)
  goal11 = isList (natList nat256M)
  goal12 = isList (natList nat1G)
  
  main = goal5
  */

  using namespace sprite;

  #define MODULE_7reverse_TP_3Nat                                 \
      (module::reverse, ((ZeroNode, "O", 0))((SuccNode, "S", 1))) \
    /**/
  TYPE(MODULE_7reverse_TP_3Nat)

  OPERATION(MyAddNode, "add", 2
    , (DT_BRANCH, RDX[0], MODULE_7reverse_TP_3Nat
        , (DT_LEAF, REWRITE(FwdNode, RDX[1]))
        , (DT_LEAF, REWRITE(SuccNode, NODE(MyAddNode, IND[0], RDX[1])))
        )
    )

  OPERATION(DoubleNode, "double", 1
    , (DT_LEAF, REWRITE(MyAddNode, RDX[0], RDX[0]))
    )

  OPERATION(MultNode, "mult", 2
    , (DT_BRANCH, RDX[0], MODULE_7reverse_TP_3Nat
        , (DT_LEAF, REWRITE(ZeroNode))
        , (DT_LEAF, REWRITE(MyAddNode, RDX[1], NODE(MultNode, IND[0], RDX[1])))
        )
    )

  OPERATION(Two, "two", 0, (DT_LEAF, REWRITE(SuccNode, NODE(SuccNode, NODE(ZeroNode)))))
  OPERATION(Four, "four", 0 , (DT_LEAF, REWRITE(DoubleNode, NODE(Two))))
  OPERATION(Nat16, "nat16", 0 , (DT_LEAF, REWRITE(MultNode, NODE(Four), NODE(Four))))
  OPERATION(Nat256, "nat256", 0 , (DT_LEAF, REWRITE(MultNode, NODE(Nat16), NODE(Nat16))))
  OPERATION(Nat4096, "nat4096", 0 , (DT_LEAF, REWRITE(MultNode, NODE(Nat256), NODE(Nat16))))
  OPERATION(Nat16384, "nat16384", 0 , (DT_LEAF, REWRITE(MultNode, NODE(Nat4096), NODE(Four))))
  OPERATION(Nat256k, "nat256k", 0 , (DT_LEAF, REWRITE(MultNode, NODE(Nat16384), NODE(Nat16))))
  OPERATION(Nat1M, "nat1M", 0 , (DT_LEAF, REWRITE(MultNode, NODE(Nat256k), NODE(Four))))
  OPERATION(Nat16M, "nat16M", 0 , (DT_LEAF, REWRITE(MultNode, NODE(Nat1M), NODE(Nat16))))
  OPERATION(Nat256M, "nat256M", 0 , (DT_LEAF, REWRITE(MultNode, NODE(Nat16M), NODE(Nat16))))
  OPERATION(Nat1G, "nat1G", 0 , (DT_LEAF, REWRITE(MultNode, NODE(Nat256M), NODE(Four))))

  #define MODULE_7reverse_TP_6MyBool                                      \
      (module::reverse, ((MyTrue, "MyTrue", 0))((MyFalse, "MyFalse", 0))) \
    /**/
  TYPE(MODULE_7reverse_TP_6MyBool)

  OPERATION(RevNode, "rev", 1
    , (DT_BRANCH, RDX[0], SPRITE_LIB_List
        , (DT_LEAF, REWRITE(lib::Nil))
        , (DT_LEAF
            , REWRITE(
                  lib::append
                , NODE(RevNode, IND[1])
                , NODE(lib::Cons, IND[0]
                , lib::nil)
                )
            )
        )
    )

  OPERATION(NatListNode, "natList", 1
    , (DT_BRANCH, RDX[0], MODULE_7reverse_TP_3Nat
        , (DT_LEAF, REWRITE(lib::Nil))
        , (DT_LEAF, REWRITE(
              lib::Cons
            , NODE(SuccNode, IND[0])
            , NODE(NatListNode, IND[0]))
            )
        )
    )

  OPERATION(IsListNode, "isList", 1
    , (DT_BRANCH, RDX[0], SPRITE_LIB_List
        , (DT_LEAF, REWRITE(MyTrue))
        , (DT_LEAF, REWRITE(IsListNode, IND[1]))
        )
    )

  OPERATION(Goal1Node, "goal1", 0
    , (DT_LEAF, REWRITE(RevNode, NODE(NatListNode, NODE(Two))))
    )

  OPERATION(Goal2Node, "goal2", 0
    , (DT_LEAF, REWRITE(RevNode, NODE(NatListNode, NODE(Nat256))))
    )

  OPERATION(Goal3Node, "goal3", 0
    , (DT_LEAF, REWRITE(IsListNode, NODE(RevNode, NODE(NatListNode, NODE(Nat4096)))))
    )

  OPERATION(Goal4Node, "goal4", 0
    , (DT_LEAF, REWRITE(IsListNode, NODE(RevNode, NODE(NatListNode, NODE(Nat16384)))))
    )

  OPERATION(Goal5aNode, "goal5a", 0
    , (DT_LEAF, REWRITE(IsListNode, NODE(NatListNode, NODE(DoubleNode, NODE(Two)))))
    )

  OPERATION(Goal5Node, "goal5", 0
    , (DT_LEAF, REWRITE(IsListNode, NODE(NatListNode, NODE(Nat16384))))
    )

  OPERATION(Goal6Node, "goal6", 0
    , (DT_LEAF, REWRITE(IsListNode, NODE(NatListNode, NODE(Nat16))))
    )

  OPERATION(Goal7Node, "goal7", 0
    , (DT_LEAF, REWRITE(IsListNode, NODE(NatListNode, NODE(Two))))
    )

  OPERATION(Goal8Node, "goal8", 0
    , (DT_LEAF, REWRITE(IsListNode, NODE(NatListNode, NODE(Nat256k))))
    )

  OPERATION(Goal9Node, "goal9", 0
    , (DT_LEAF, REWRITE(IsListNode, NODE(NatListNode, NODE(Nat1M))))
    )

  OPERATION(Goal10Node, "goal10", 0
    , (DT_LEAF, REWRITE(IsListNode, NODE(NatListNode, NODE(Nat16M))))
    )

  OPERATION(Goal11Node, "goal11", 0
    , (DT_LEAF, REWRITE(IsListNode, NODE(NatListNode, NODE(Nat256M))))
    )

  OPERATION(Goal12Node, "goal12", 0
    , (DT_LEAF, REWRITE(IsListNode, NODE(NatListNode, NODE(Nat1G))))
    )

  OPERATION(MainNode, "main", 0, (DT_LEAF, REWRITE(Goal10Node)))
}}}
