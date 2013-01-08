#pragma once
#include "sprite/sprite.hpp"

namespace tree_insert_32
{
  /*
    data BT = Leaf | Branch Int BT BT
    insert x Leaf = Branch x Leaf Leaf
    insert x (Branch y l r) | x < y = Branch y (insert x l) r
                            | y < x = Branch y l (insert x r)
    			| x == y = Branch y l r
    
    count Leaf = 0
    count (Branch _ l r) = 1 + count l + count r
    
    tree_loop n x t = if n==0 then t
       else tree_loop (n-1) (next x) (insert x t)
    
    -- count so it does not print a big tree
    main = count (tree_loop COUNT SEED Leaf)
    
    
    
    -- Generates a pseudo-random sequence  using two LCS generators.
    -- Each generator contributes about half of the number in the sequence.
    -- This enable relatively long sequences with only 32 bits arithmetic.
    
    -- Mathematics of Computation
    -- Volume 68, Number 225, January 1999, Pages 249-260
    -- S 0025-5718(99)00996-5
    -- Tables of Linear Congruential Generators
    -- of Different Sizes and Good Lattice Structure
    -- Pierre L'Ecuyer
    
    -- Generator 1
    M1 = 16381
    A1 = 3007
    -- max intermediate value before mod ~ 49,257,667  < 2^26
    
    -- Generator 2
    M2 = 32749
    A2 = 1944
    -- max intermediate value before mod ~ 63,664,056 < 2^26
    
    HALFSEED = 15809     -- must be less than M2
    COUNT = 200000
    -- max number of distinct pairs = M1*M2 = 536,461,369 ~ 2^29
    
    SEED = HALFSEED*M2+HALFSEED
    
    -- think of x as a 2-digit number in base M2
    -- get the digits, find the next of each digit
    -- and put the digits back into a single number
    next x = y
      where p = div x M2
            q = mod x M2
            r = mod (A1 * p) M1
            s = mod (A2 * q) M2
            y = r * M2 + s
  */

  using namespace sprite;

  #define TP_Tree ((LeafNode, "Leaf", 0))((BranchNode, "Branch", 3))
  TYPE(TP_Tree)
  
  STATIC_NODE(leaf, LeafNode);
  STATIC_NODE(zero, IntNode, 0);
  STATIC_NODE(one, IntNode, 1);
  STATIC_NODE(M1, IntNode, 16381);
  STATIC_NODE(A1, IntNode, 3007);
  STATIC_NODE(M2, IntNode, 32749);
  STATIC_NODE(A2, IntNode, 1944);
  STATIC_NODE(HALFSEED, IntNode, 15809);
  STATIC_NODE(COUNT, IntNode, 200000);

  OPERATION(SEEDNode, "SEED", 0
    , (DT_LEAF, REWRITE(AddNode, NODE(MulNode, HALFSEED, M2), HALFSEED))
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

  // ====== next ======
  OPERATION(NextNode, "next", 1
    , (DT_LEAF, REWRITE(
          AddNode
            , NODE(
                  MulNode
                , NODE( // r
                      ModNode
                    , NODE(
                          MulNode
                        , A1
                        , NODE(DivNode, RDX[0], M2) // p
                        )
                    , M1
                    )
                , M2
                )
            , NODE( // s
                  ModNode
                , NODE(
                      MulNode
                    , A2
                    , NODE(ModNode, RDX[0], M2) // q
                    )
                , M2
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
                , NODE(NextNode, RDX[1])
                , NODE(InsertNode, RDX[1], RDX[2])
                )
            )
        )
    )

  // ====== main ======
  OPERATION(MainNode, "main", 0
    , (DT_LEAF, REWRITE(CountNode, NODE(TreeLoopNode, COUNT, NODE(SEEDNode), leaf))))
}

