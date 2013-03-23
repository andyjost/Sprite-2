#pragma once
#include "sprite/sprite.hpp"
#include "sprite/lib/int.hpp"

namespace sprite { namespace module { namespace tree_insertn
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
  using lib::constants::i0;
  using lib::constants::i1;

  #define MODULE_12tree_insertn_TP_4Tree                                 \
      (module::tree_insertn, ((Leaf, "Leaf", 0))((Branch, "Branch", 3))) \
    /**/
  TYPE(MODULE_12tree_insertn_TP_4Tree)
  
  STATIC_NODE(leaf, Leaf);
  STATIC_NODE(m, IntNode, 39916801);
  STATIC_NODE(a_, IntNode, 1664525);
  STATIC_NODE(b, IntNode, 1013904223);
  STATIC_NODE(iterations, IntNode, 200000);
  STATIC_NODE(someseed, IntNode, 24642);
  STATIC_NODE(modval, IntNode, 200000);

  OPERATION(RndNode, "rnd", 1
    , (DT_LEAF, REWRITE(ModNode, NODE(AddNode, NODE(MulNode, a_, RDX[0]), b), m))
    )

  // The change below makes performance worse.
  #if 0
  namespace eager
  {
    int64 count(NodePtr x)
    {
      static void * table[] = { &&nil, &&cons };

      x = H(x);
      assert(x->tag >= CTOR);
      goto *table[x->tag];

    nil:
      return 0;

    cons:
      return 1 + count(x->at(1)) + count(x->at(2));
    }
  }

  // ====== count ======
  OPERATION(CountNode, "count", 1
    , (DT_LEAF
        , int64 result;

          BEGIN_EAGER
            result = eager::count(RDX[0]);
          END_EAGER

          REWRITE(IntNode, result);
        )
    )
  #endif

  OPERATION(CountNode, "count", 1
    , (DT_BRANCH, RDX[0], MODULE_12tree_insertn_TP_4Tree
        , (DT_LEAF, REWRITE(IntNode, 0L))
        , (DT_LEAF, REWRITE(
              AddNode, i1, NODE(AddNode
                , NODE(CountNode, IND[1])
                , NODE(CountNode, IND[2]))
                )
            )
        )
    )
      
  struct InsertNode;

  namespace eager
  {
    NodePtr insert(NodePtr x, NodePtr ylr)
    {
        static void * table[] = { &&nil, &&cons };

        ylr = H(ylr);
        assert(ylr->tag >= CTOR);
        goto *table[ylr->tag];

      nil:
        return NODE(Branch, x, leaf, leaf);

      cons:
      {
        int64 x_ = unbox(x);
        int64 y_ = unbox(ylr->at(0));
        if(x_ < y_)
          return NODE(
              Branch, ylr->at(0), NODE(InsertNode, x, ylr->at(1)), ylr->at(2)
            );
        else if(y_ < x_)
          return NODE(
              Branch, ylr->at(0), ylr->at(1), NODE(InsertNode, x, ylr->at(2))
            );
        else if(x_ == y_)
          return ylr;
      }
      throw RuntimeError();
    }
  }

  // ====== insert ======
  OPERATION(InsertNode, "insert", 2
    , (DT_LEAF
        , NodePtr result;

          BEGIN_EAGER
            result = eager::insert(RDX[0], RDX[1]);
          END_EAGER

          REWRITE(FwdNode, result);
        )
    )

  // ====== tree_loop ======
  OPERATION(TreeLoopNode, "tree_loop", 3
    , (DT_LEAF
        , IF(
              NODE(EqNode, RDX[0], i0)
            , THEN(FwdNode, RDX[2])
            , ELSE(
                  TreeLoopNode
                , NODE(SubNode, RDX[0], i1)
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
}}}

