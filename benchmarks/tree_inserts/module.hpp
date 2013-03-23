#pragma once
#include "sprite/sprite.hpp"
#include "sprite/lib/int.hpp"

namespace sprite { namespace module { namespace tree_inserts
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

#define MODULE_12tree_insert_TP_4Tree                                   \
      (module::tree_inserts, ((Leaf, "Leaf", 0))((Branch, "Branch", 3))) \
      /**/
      TYPE(MODULE_12tree_insert_TP_4Tree)
  
      STATIC_NODE(leaf, Leaf);
      STATIC_NODE(m, IntNode, 39916801);
      STATIC_NODE(a_, IntNode, 1664525);
      STATIC_NODE(b, IntNode, 1013904223);
      STATIC_NODE(iterations, IntNode, 200000);
      STATIC_NODE(someseed, IntNode, 24642);
      STATIC_NODE(modval, IntNode, 200000);

      int64 count01(NodePtr t) {
        static void * utable[] = { &&fail, &&fwd, &&choice, &&oper , &&leaf , &&branch };
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

      struct CountNode : Node 
      {
        CountNode( NodePtr const & arg0) : Node(OPER) 
        {
          new(&args[0]) NodePtr(arg0);
        }
        CountNode( Partial<CountNode,0> * part , NodePtr const & arg0 ) : Node(OPER) 
        {
          uninitialized_copy(this, part);
          new(&args[0]) NodePtr(arg0);
        }
        virtual ~CountNode() 
        {
          (*reinterpret_cast<NodePtr *>(&this->args[0])).~NodePtr();
        }
        void * args[1];
        typedef CountNode this_type;
        typedef mpl::size_t<1> arity_value;
        static std::string static_name() 
        {
          return "count";
        }
        virtual std::string name() 
        {
          return static_name();
        }
        virtual void show() 
        {
          if(1 > 0) std::cout << "(";
          std::cout << static_name();
          std::cout << " ";
          (*reinterpret_cast<NodePtr *>(&this->args[0]))->show();
          if(1 > 0) std::cout << ")";
        }
        virtual size_t arity() const 
        {
          return arity_value::value;
        }
        inline NodePtr & operator[](size_t i) 
        {
          (static_cast<void> (0));
          return (*reinterpret_cast<NodePtr *>(&this->args[i]));
        }
        virtual NodePtr & at(size_t i) 
        {
          return (*this)[i];
        }
        virtual void N() 
        {
          // count returns an int.  H and N are the same
          H();
        }
        virtual void H() 
        {
          int64 tmp = count01(get((*static_cast<this_type*>(g_redex))[0]));
          // std::cout << "count exit "; std::cout << tmp; std::cout << std::endl;
          g_redex = this;
          rewrite<IntNode>(tmp);
          // it is an int, no further call to H
        }
      };

      struct RndNode : Node 
      {
        RndNode( NodePtr const & arg0) : Node(OPER) 
        {
          new(&args[0]) NodePtr(arg0);
        }
        RndNode( Partial<RndNode,0> * part , NodePtr const & arg0 ) : Node(OPER) 
        {
          uninitialized_copy(this, part);
          new(&args[0]) NodePtr(arg0);
        }
        virtual ~RndNode() 
        {
          (*reinterpret_cast<NodePtr *>(&this->args[0])).~NodePtr();
        }
        void * args[1];
        typedef RndNode this_type;
        typedef mpl::size_t<1> arity_value;
        static std::string static_name() 
        {
          return "rnd";
        }
        virtual std::string name() 
        {
          return static_name();
        }
        virtual void show() 
        {
          if(1 > 0) std::cout << "(";
          std::cout << static_name();
          std::cout << " ";
          (*reinterpret_cast<NodePtr *>(&this->args[0]))->show();
          if(1 > 0) std::cout << ")";
        }
        virtual size_t arity() const 
        {
          return arity_value::value;
        }
        inline NodePtr & operator[](size_t i) 
        {
          (static_cast<void> (0));
          return (*reinterpret_cast<NodePtr *>(&this->args[i]));
        }
        virtual NodePtr & at(size_t i) 
        {
          return (*this)[i];
        }
        virtual void N() 
        {
          // N() and H() are the same for integers
          H();
        }
        virtual void H() 
        {
          NodePtr x = RDX[0];
          g_redex = this;
          // std::cout << "rnd01 "; x->show(); std::cout << std::endl;
          static void * utable[] = { &&fail, &&fwd, &&choice, &&oper , &&ctor };
          static void ** table = &utable[4];
          goto *table[x->tag];
        fail:
          throw RuntimeError("Unexpected fail in rnd01");
        choice:
          throw RuntimeError("Unexpected choice in rnd01");
        fwd:
          x = (static_cast<FwdNode *>(get(x)))->dest;
          goto *table[x->tag];
        oper:
          x->H();
          goto *table[x->tag];
        ctor:
          int64 vx = x->value();
          int64 tmp = (1664525 * vx + 1013904223) % 39916801;
          // std::cout << "rnd01 exit "; std::cout << tmp; std::cout << std::endl;
          rewrite<IntNode>(tmp);
        }
      };

      //    OPERATION(RndNode, "rnd", 1
      //              , (DT_LEAF, REWRITE(ModNode, NODE(AddNode, NODE(MulNode, a_, RDX[0]), b), m))
      //    )

      //  // ====== count ======
      //  OPERATION(CountNode, "count", 1
      //    , (DT_BRANCH, RDX[0], MODULE_12tree_insert_TP_4Tree
      //        , (DT_LEAF, REWRITE(IntNode, 0L))
      //        , (DT_LEAF, REWRITE(
      //              AddNode, i1, NODE(AddNode
      //                , NODE(CountNode, IND[1])
      //                , NODE(CountNode, IND[2]))
      //                )
      //            )
      //        )
      //    )

      struct InsertNode;

      NodePtr insert01(NodePtr x, NodePtr t) {
        static void * utable[] = { &&fail, &&fwd, &&choice, &&oper , &&leaf , &&branch };
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
        return sprite::construct<Branch>(x, leaf, leaf);
      branch:
        int64 x_ = unbox(x);
        int64 y_ = unbox(t->at(0));
        if(x_ < y_)
          return sprite::construct<Branch>(t->at(0), sprite::construct<InsertNode>(x, t->at(1)), t->at(2));
        else if(y_ < x_)
          return sprite::construct<Branch>(t->at(0), t->at(1), sprite::construct<InsertNode>(x, t->at(2)));
        else if(x_ == y_)
          return t;
        else throw RuntimeError("Impossible condition in insert01");
      }
    
      struct InsertNode : Node 
      {
        InsertNode( NodePtr const & arg0 , NodePtr const & arg1) : Node(OPER) 
        {
          new(&args[0]) NodePtr(arg0);
          new(&args[1]) NodePtr(arg1);
        }
        InsertNode( Partial<InsertNode,1> * part , NodePtr const & arg1 ) : Node(OPER) 
        {
          uninitialized_copy(this, part);
          new(&args[1]) NodePtr(arg1);
        }
        virtual ~InsertNode() 
        {
          (*reinterpret_cast<NodePtr *>(&this->args[0])).~NodePtr();
          (*reinterpret_cast<NodePtr *>(&this->args[1])).~NodePtr();
        }
        void * args[2];
        typedef InsertNode this_type;
        typedef mpl::size_t<2> arity_value;
        static std::string static_name() 
        {
          return "insert";
        }
        virtual std::string name() 
        {
          return static_name();
        }
        virtual void show() 
        {
          if(2 > 0) std::cout << "(";
          std::cout << static_name();
          std::cout << " ";
          (*reinterpret_cast<NodePtr *>(&this->args[0]))->show();
          std::cout << " ";
          (*reinterpret_cast<NodePtr *>(&this->args[1]))->show();
          if(2 > 0) std::cout << ")";
        }
        virtual size_t arity() const 
        {
          return arity_value::value;
        }
        inline NodePtr & operator[](size_t i) 
        {
          (static_cast<void> (0));
          return (*reinterpret_cast<NodePtr *>(&this->args[i]));
        }
        virtual NodePtr & at(size_t i) 
        {
          return (*this)[i];
        }
        virtual void N() 
        {
          g_redex = this;
          dt();
          N();
        }
        virtual void H() 
        {
          g_redex = this;
          NodePtr a0 = RDX[0];
          // std::cout << "insert01 arg0 "; a0->show(); std::cout << std::endl;
          NodePtr a1 = RDX[1];
          // std::cout << "insert01 arg1 "; a1->show(); std::cout << std::endl;
          NodePtr tmp = insert01(a0,a1);
          // std::cout << "insert01 exit "; tmp->show(); std::cout << std::endl;
          g_redex = this;
          rewrite<FwdNode>(tmp);
        }
      private: __attribute__((noinline)) static void dt() 
        {
          typedef this_type inductive_type;
          static procedure vtable[] = 
            {
              &fail, &fwd, &choice, &oper , &this_type::dt_0 , &this_type::dt_1 
            }
          ;
          g_inductive = get((*static_cast<this_type*>(g_redex))[1]);
          g_vtable = &vtable[4];
          g_vtable[g_inductive->tag]();
        }
        static void dt_0() 
        {
          typedef sprite::module::tree_inserts::Leaf inductive_type;
          rewrite<Branch>((*static_cast<this_type*>(g_redex))[0], leaf, leaf);
        }
        static void dt_1() 
        {
          typedef sprite::module::tree_inserts::Branch inductive_type;
          rewrite<lib::ifThenElse>(sprite::construct<LtNode>((*static_cast<this_type*>(g_redex))[0], (ind<inductive_type>())[0]), sprite::construct<Branch>((ind<inductive_type>())[0], sprite::construct<InsertNode>((*static_cast<this_type*>(g_redex))[0], (ind<inductive_type>())[1]), (ind<inductive_type>())[2]), sprite::construct<lib::ifThenElse>(sprite::construct<LtNode>((ind<inductive_type>())[0], (*static_cast<this_type*>(g_redex))[0]), sprite::construct<Branch>((ind<inductive_type>())[0], (ind<inductive_type>())[1], sprite::construct<InsertNode>((*static_cast<this_type*>(g_redex))[0], (ind<inductive_type>())[2])), sprite::construct<Branch>((ind<inductive_type>())[0], (ind<inductive_type>())[1], (ind<inductive_type>())[2])));
        }
      };

      /*
      // ====== insert ======
      OPERATION(InsertNode, "insert", 2
                , (DT_BRANCH, RDX[1], MODULE_12tree_insert_TP_4Tree
                   , (DT_LEAF, REWRITE(Branch, RDX[0], leaf, leaf))
                   , (DT_LEAF
                      , IF(
                           NODE(LtNode, RDX[0], IND[0])
                           , THEN(Branch, IND[0], NODE(InsertNode, RDX[0], IND[1]), IND[2])
                           , ELIF(
                                  NODE(LtNode, IND[0], RDX[0])
                                  , THEN(Branch, IND[0], IND[1], NODE(InsertNode, RDX[0], IND[2]))
                                  , ELSE(Branch, IND[0], IND[1], IND[2])
                                  )
                           )
                      )
                   )
                )
      */

      struct TreeLoopNode;

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
          //      return treeloop01(n-1, NODE(RndNode,x), insert01(NODE(ModNode,x,iterations),t));
        }
      }

      struct TreeLoopNode : Node 
      {
        TreeLoopNode( NodePtr const & arg0 , NodePtr const & arg1 , NodePtr const & arg2) : Node(OPER) 
        {
          args = reinterpret_cast<void**>(child_allocator[3].malloc());
          new(&args[0]) NodePtr(arg0);
          new(&args[1]) NodePtr(arg1);
          new(&args[2]) NodePtr(arg2);
        }
        TreeLoopNode( Partial<TreeLoopNode,2> * part , NodePtr const & arg2 ) : Node(OPER) 
        {
          args = reinterpret_cast<void**>(child_allocator[3].malloc());
          uninitialized_copy(this, part);
          new(&args[2]) NodePtr(arg2);
        }
        virtual ~TreeLoopNode() 
        {
          (*reinterpret_cast<NodePtr *>(&this->args[0])).~NodePtr();
          (*reinterpret_cast<NodePtr *>(&this->args[1])).~NodePtr();
          (*reinterpret_cast<NodePtr *>(&this->args[2])).~NodePtr();
          child_allocator[3].free(args);
        }
        void ** args;
        typedef TreeLoopNode this_type;
        typedef mpl::size_t<3> arity_value;
        static std::string static_name() 
        {
          return "tree_loop";
        }
        virtual std::string name() 
        {
          return static_name();
        }
        virtual void show() 
        {
          if(3 > 0) std::cout << "(";
          std::cout << static_name();
          std::cout << " ";
          (*reinterpret_cast<NodePtr *>(&this->args[0]))->show();
          std::cout << " ";
          (*reinterpret_cast<NodePtr *>(&this->args[1]))->show();
          std::cout << " ";
          (*reinterpret_cast<NodePtr *>(&this->args[2]))->show();
          if(3 > 0) std::cout << ")";
        }
        virtual size_t arity() const 
        {
          return arity_value::value;
        }
        inline NodePtr & operator[](size_t i) 
        {
          (static_cast<void> (0));
          return (*reinterpret_cast<NodePtr *>(&this->args[i]));
        }
        virtual NodePtr & at(size_t i) 
        {
          return (*this)[i];
        }
        virtual void N() 
        {
          g_redex = this;
          dt();
          N();
        }
        virtual void H() 
        {
          g_redex = this;
          int64 n = RDX[0]->value();
          NodePtr tmp = treeloop01(n, RDX[1], RDX[2]);
          g_redex = this;
          // Watch the callee if you remove FwdNode !!!
          rewrite<FwdNode>(tmp);
        }
      private: __attribute__((noinline)) static void dt() 
        {
          typedef this_type inductive_type;
          rewrite<lib::ifThenElse>(sprite::construct<EqNode>((*static_cast<this_type*>(g_redex))[0], i0), sprite::construct<FwdNode>((*static_cast<this_type*>(g_redex))[2]), sprite::construct<TreeLoopNode>(sprite::construct<SubNode>((*static_cast<this_type*>(g_redex))[0], i1) , sprite::construct<RndNode>((*static_cast<this_type*>(g_redex))[1]) , sprite::construct<InsertNode>(sprite::construct<ModNode>((*static_cast<this_type*>(g_redex))[1], modval), (*static_cast<this_type*>(g_redex))[2])));
        }
      };


      /*

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

      */

     struct MainNode : Node 
     {
     MainNode() : Node(OPER) 
     {
     }
     virtual ~MainNode() 
     {
     }
     void * args[0];
     typedef MainNode this_type;
     typedef mpl::size_t<0> arity_value;
     static std::string static_name() 
     {
     return "main";
     }
     virtual std::string name() 
     {
     return static_name();
     }
     virtual void show() 
     {
     if(0 > 0) std::cout << "(";
     std::cout << static_name();
     if(0 > 0) std::cout << ")";
     }
     virtual size_t arity() const 
     {
     return arity_value::value;
     }
     virtual void N() 
     {
     g_redex = this;
     dt();
     N();
     }
     virtual void H() 
     {
     g_redex = this;
     dt();
     H();
     }
     private: __attribute__((noinline)) static void dt() 
     {
     typedef this_type inductive_type;
     rewrite<CountNode>(sprite::construct<TreeLoopNode>(iterations, someseed, leaf));
     }
     }
     ;

//     // ====== main ======
//     OPERATION(MainNode, "main", 0
//              , (DT_LEAF, REWRITE(CountNode, NODE(TreeLoopNode, iterations, someseed, leaf)))
//              )
   }}}
