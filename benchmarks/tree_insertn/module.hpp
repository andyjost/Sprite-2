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

  namespace eager
  {
    int64 count01(NodePtr t) {
      static void * utable[] = {
          &&fail, &&fwd, &&choice, &&oper , &&leaf , &&branch
        };
      static void ** table = &utable[4];
      goto *table[t->tag];
    fail:
      throw RuntimeError("Unexpected fail in count01");
    choice:
      throw RuntimeError("Unexpected choice in count01");
    fwd:
      t = (static_cast<FwdNode *>(get(t)))->dest;
      goto *table[t->tag];
    oper:
      t->H();
      goto *table[t->tag];
    leaf:
      return 0;
    branch:
      return 1 + count01(t->at(1)) + count01(t->at(2));
    }
  }

  // ====== count ======
  OPERATION(CountNode, "count", 1
    , (DT_LEAF
        , int64 tmp;
          BEGIN_EAGER
            tmp = eager::count01(RDX[0]);
          END_EAGER

          REWRITE(IntNode, tmp);
        )
    )

  struct InsertNode;

  namespace eager
  {
    NodePtr insert01(NodePtr x, NodePtr t) {
      static void * utable[] = {
          &&fail, &&fwd, &&choice, &&oper , &&leaf , &&branch
        };
      static void ** table = &utable[4];
      goto *table[t->tag];
    fail:
      throw RuntimeError("Unexpected fail in insert01");
    choice:
      throw RuntimeError("Unexpected choice in insert01");
    fwd:
      t = (static_cast<FwdNode *>(get(t)))->dest;
      goto *table[t->tag];
    oper:
      t->H();
      goto *table[t->tag];
    leaf:
      // return sprite::construct<Branch>(x, leaf, leaf);
      return NODE(Branch, x, leaf, leaf);
    branch:
      int64 x_ = unbox(x);
      int64 y_ = unbox(t->at(0));
      if(x_ < y_)
        // return sprite::construct<Branch>(t->at(0), sprite::construct<InsertNode>(x, t->at(1)), t->at(2));
        return NODE(Branch, t->at(0), NODE(InsertNode, x, t->at(1)), t->at(2));
      else if(y_ < x_)
        // return sprite::construct<Branch>(t->at(0), t->at(1), sprite::construct<InsertNode>(x, t->at(2)));
        return NODE(Branch, t->at(0), t->at(1), NODE(InsertNode, x, t->at(2)));
      else if(x_ == y_)
        return t;
      else throw RuntimeError("Impossible condition in insert01");
    }
  }

  // ====== insert ======
  OPERATION(InsertNode, "insert", 2
    , (DT_LEAF
        , NodePtr tmp;
          BEGIN_EAGER
            NodePtr a0 = RDX[0];
            NodePtr a1 = RDX[1];
            tmp = eager::insert01(a0,a1);
          END_EAGER
          REWRITE(FwdNode, tmp);
        )
    )

  // ====== tree_loop ======
  struct TreeLoopNode;

  namespace eager
  {
    NodePtr treeloop01(int64 n, NodePtr x, NodePtr t) {
    start:
      if (n==0) {
        // This should be a forward node,
        // but is it forwarded by the caller --- MESSY
        return t;
      } else {
        // tail recursion
        n = n-1;
        // Watch out the order !!!
        t = insert01(NODE(ModNode,x,iterations),t);
        x = NODE(RndNode,x);
        goto start;
      }
    }
  }

  OPERATION(TreeLoopNode, "tree_loop", 3
    , (DT_LEAF
        , NodePtr tmp;
          BEGIN_EAGER
            int64 n = RDX[0]->value();
            tmp = eager::treeloop01(n, RDX[1], RDX[2]);
          END_EAGER
          // Watch the callee if you remove FwdNode !!!
          REWRITE(FwdNode, tmp);
        )
    )

  // ====== main ======
  OPERATION(MainNode, "main", 0
    , (DT_LEAF, REWRITE(CountNode, NODE(TreeLoopNode, iterations, someseed, leaf)))
    )
}}}

