#pragma once
#include "sprite/sprite.hpp"

namespace tree_insert
{
  /*
    m = 39916801
    a = 1664525
    b = 1013904223
    rnd x = (a * x + b) `mod` m
    
    data BT = Leaf | Branch Int BT BT
    insert x Leaf = Branch x Leaf Leaf
    insert x (Branch y l r) | x < y = Branch y (insert x l) r
                            | y < x = Branch y l (insert x r)
          | x == y = Branch y l r
    
    count Leaf = 0
    count (Branch _ l r) = 1 + count l + count r
    
    tree_loop n x t = if n==0 then t
       else tree_loop (n-1) (rnd x) (insert (x`mod`200000) t)
    
    iterations = 200000
    someseed = 24642
    
    -- count so it does not print a big tree
    main = count (tree_loop iterations someseed Leaf)
  */
  using namespace sprite;

  #define TP_Tree ((LeafNode, "Leaf", 0))((BranchNode, "Branch", 3))
  TYPE(TP_Tree)
  
  STATIC_NODE(leaf, LeafNode);
  STATIC_NODE(zero, IntNode, 0);
  STATIC_NODE(one, IntNode, 1);
  STATIC_NODE(m, IntNode, 39916801);
  STATIC_NODE(a_, IntNode, 1664525);
  STATIC_NODE(b, IntNode, 1013904223);
  STATIC_NODE(iterations, IntNode, 200000);
  STATIC_NODE(someseed, IntNode, 24642);
  STATIC_NODE(modval, IntNode, 200000);

  OPERATION(RndNode, "rnd", 1
    , (DT_LEAF, REWRITE(ModNode, NODE(AddNode, NODE(MulNode, a_, RDX[0]), b), m))
    )

  // ====== count ======
  OPERATION(CountNode, "count", 1
    , (DT_BRANCH, RDX[0], TP_Tree
        , (DT_LEAF, REWRITE(IntNode, 0L))
        , (DT_LEAF, REWRITE(
              AddNode, one, NODE(AddNode
                , NODE(CountNode, IND[1])
                , NODE(CountNode, IND[2]))
                )
            )
        )
    )
      
  // ====== insert ======
  OPERATION(InsertNode, "insert", 2
    , (DT_BRANCH, RDX[1], TP_Tree
        , (DT_LEAF, REWRITE(BranchNode, RDX[0], leaf, leaf))
        , (DT_LEAF
            , COND(
                  LtNode(RDX[0], IND[0])
                , REWRITE(BranchNode, IND[0], NODE(InsertNode, RDX[0], IND[1]), IND[2])
                , COND(
                      LtNode(IND[0], RDX[0])
                    , REWRITE(BranchNode, IND[0], IND[1], NODE(InsertNode, RDX[0], IND[2]))
                    , REWRITE(BranchNode, IND[0], IND[1], IND[2])
                    )
                )
            )
        )
    )

  // ====== tree_loop ======
  OPERATION(TreeLoopNode, "tree_loop", 3
    , (DT_LEAF
        , COND(
              EqNode(RDX[0], zero)
            , REWRITE(FwdNode, RDX[2])
            , REWRITE(TreeLoopNode
                , NODE(SubNode, RDX[0], one)
                , NODE(RndNode, RDX[1])
                , NODE(InsertNode, NODE(ModNode, RDX[1], modval), RDX[2])
                )
            )
        )
    )

  // ====== main ======
  OPERATION(MainNode, "main", 0
    , (DT_LEAF, REWRITE(CountNode, NODE(TreeLoopNode, iterations, someseed, leaf)))
    )
}

