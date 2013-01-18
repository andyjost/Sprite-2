/**
 * @file
 * @brief The Sprite Node definition.
 */
#pragma once
#include "sprite/common.hpp"
#include "sprite/memory.hpp"
#include <boost/mpl/if.hpp>
#include <boost/mpl/size_t.hpp>
#include <boost/utility.hpp>

#if SPRITE_REFCNT
#include <boost/intrusive_ptr.hpp>
#endif

namespace sprite
{
  namespace mpl = boost::mpl;
  using boost::enable_if_c;
  using boost::disable_if_c;

  struct NodeH;

  struct Node
  {
    // To facilitate rewriting, the reference count must *not* be initialized.
    // The memory allocator is responsible for zero-initializing the count when
    // raw memory is obtained.
    Node(tag_type tag_) : tag(tag_) {}
    Node(Node const & arg); // disable
    Node & operator=(Node const &); // disable

    #if SPRITE_USE_DTOR
    virtual ~Node() {}
    #endif
  
    virtual char const * name() { return "Node"; }
    virtual void show() { std::cout << name(); }
    virtual void N() = 0;
    virtual void H() = 0;
    virtual int64 value() { throw RuntimeError(); }
    virtual size_t arity() const = 0;
    virtual Node * apply(Node * arg) { throw RuntimeError(); }
  
    tag_type tag;

    #if SPRITE_GC
      mark_type mark;
    #endif

    #if SPRITE_REFCNT
      refcnt_type refcnt;
    #endif

    // More expensive version of operator[] that works on Node.
    virtual NodePtr & at(size_t i) { throw RuntimeError(); }

    /**
     * @brief Allocates storage for a new Node object.
     *
     */
    void * operator new(size_t sz)
    {
      assert(sz <= NODE_BYTES);
      void * p = node_allocator->malloc();
      assert(p);
      #ifdef TRACEALLOC
        std::cout << "alloc@" << p << std::endl;
      #endif

      #if SPRITE_GC
        // Garbage collection.  New nodes are marked locked.
        reinterpret_cast<Node *>(p)->mark = LOCKED_MARK;
      #endif

      #if SPRITE_REFCNT
        // Reference counting.
        // New nodes have a reference count of zero.
        // FIXME: I don't think this is necessary.
        reinterpret_cast<Node *>(p)->refcnt = 0;
      #endif
      return p;
    }
    void * operator new(size_t sz, void * p) { return p; }

    #if SPRITE_USE_DTOR
      void operator delete(void * p)
      {
        #ifdef TRACEALLOC
          std::cout << "dealloc@" << p

          #if SPRITE_REFCNT
            << " with refcnt=" << ((Node *)p)->refcnt
          #endif

          #if SPRITE_GC
            << " with mark=" << (int)(((Node *)p)->mark)
          #endif

            << std::endl;
        #endif

        #if SPRITE_REFCNT
          assert((reinterpret_cast<Node *>(p))->refcnt == 0);
        #endif

        node_allocator->free(p);
      }
    #endif

    #if SPRITE_GC
      // For GC mode, give range-like access to the children.
      typedef NodeH * const * iterator;
      virtual iterator begin() = 0;
      virtual iterator end() = 0;
      typedef NodeH const * const * const_iterator;
      virtual const_iterator begin() const = 0;
      virtual const_iterator end() const = 0;
    #endif

    #if SPRITE_REFCNT
      // For ref-counted mode, give hooks for incrementing and decrementing the
      // count.
      friend void intrusive_ptr_add_ref(Node * node)
      {
        ++node->refcnt;
        #ifdef TRACEREFS
          std::cout << "DEBUG> @" << node << " (+) "
            << node->refcnt << " refs" << std::endl;
        #endif
      }
      friend void intrusive_ptr_release(Node * node)
      {
        assert(node->refcnt != 0);
        --node->refcnt;
        #ifdef TRACEREFS
          std::cout << "DEBUG> @" << node << " (-) "
            << node->refcnt << " refs" << std::endl;
        #endif

        #if !SPRITE_GC
          if(node->refcnt == 0) delete node;
        #endif
      }
    #endif
  };

  #if SPRITE_REFCNT
    struct NodePtr : boost::intrusive_ptr<Node>
    {
      NodePtr() {}
      NodePtr(Node * p, bool add_ref = true)
        : boost::intrusive_ptr<Node>(p, add_ref)
      {}
      NodePtr & operator[](size_t i) const { return (*this)->at(i);  }
    };

    /// Get the raw pointer (when NodePtr is a class).
    Node * get(NodePtr const & ptr) { return ptr.get(); }

  #else

    /// Get the raw pointer (when NodePtr is a raw pointer).
    Node * get(Node * ptr) { return ptr; }
  #endif

  // A freestanding apply function.
  inline NodePtr apply(NodePtr const & func, NodePtr const & arg)
    { return func->apply(get(arg)); }

  inline NodePtr apply(Node & func, NodePtr const & arg)
    { return func.apply(get(arg)); }

  /**
   * @brief Primarty template declaration for partial nodes.
   *
   * The template arguments are defined as follows:
   *
   *   CompleteType
   *       The type of the complete node.  After all arguments have been bound,
   *       an instnace of that type is constructed.
   *
   *   NumBound
   *       The number of bound arguments, where:
   *
   *           0 <= NumBound < CompleteType::arity_value::value
   */
  template<typename CompleteType, size_t NumBound> struct Partial;

  /// Copies successors from the source to destination.  
  template<typename T, typename U>
  void copy_successors(T * dst, U * src
    , typename enable_if_c<
          (T::arity_value::value > 0 && U::arity_value::value > 0)
        >::type* = 0
    )
  {
    // TODO - Use SIMD and static unrolling.
    BOOST_STATIC_ASSERT(T::arity_value::value >= U::arity_value::value);
    for(size_t i=0; i<U::arity_value::value; ++i)
      { (*dst)[i] = (*src)[i]; }
  }

  template<typename T, typename U>
  void copy_successors(T * dst, U * src
    , typename disable_if_c<
          (T::arity_value::value > 0 && U::arity_value::value > 0)
        >::type* = 0
    )
  {
  }

  #if SPRITE_GC
    /**
     * @brief A node pointer that prevents collection.
     *
     * Since the collector can run at any time, we need a way to prevent
     * collection of nodes that are allocated but not yet attached to the root.
     * This class holds a newly-allocated node.  Upon construction, the
     * allocator marks nodes as new and upon desctruction, this class assigns
     * them the current mark (stored in g_mark).  If the collector runs before
     * this object is destroyed, then the new mark will prevent the associated
     * node from being collected.  This is also useful for creating static node
     * constants, since they might not be related to the root, either.
     *
     * There is an implicit conversion to Node *, so this can be used as a
     * drop-in replacement for Node * in many cases.
     */
    struct LockedPtr : boost::noncopyable
    {
      explicit LockedPtr(Node * p) : px(p)
        { assert(px->mark == LOCKED_MARK); }
      ~LockedPtr()
      {
        // std::cout << "unlocking " << get(px) << " (mark = " << (int)g_mark
        //   << ")" << std::endl;
        px->mark = g_mark;
      }
      operator NodePtr const &() const { return px; }
      NodePtr const & operator->() const { return px; }
      Node & operator*() const { return *px; }
    private:
      NodePtr px;
    };
  #else
  #endif

  /// Global pointer to the root of the goal expression (used for tracing).
  extern NodePtr root;

  void trace()
  {
    #ifdef TRACING
      std::cout << "TRACE> "; root->show(); std::cout << std::endl;
    #endif
  }

  // With reference counting, the arguments must be taken by value to force a
  // copy.  This avoids freeing them early when the destructor is called.  With
  // garbage collection, the NodePtr is just a Node *, so the copy is free.
  #define F(z,n,_)                                           \
      template<typename TgtType>                             \
      inline void rewrite(BOOST_PP_ENUM_PARAMS(n,NodePtr a)) \
      {                                                      \
        g_redex->~Node(); /* optimized away if no dtor */    \
        new(g_redex) TgtType(BOOST_PP_ENUM_PARAMS(n,a));     \
        trace();                                             \
      }                                                      \
    /**/
  BOOST_PP_REPEAT(SPRITE_ARITY_BOUND,F,)
  #undef F

  template<typename TgtType>
  inline void rewrite(int64 value)
  {
    g_redex->~Node(); // optimized away if no dtor
    new(g_redex) TgtType(value);
    trace();
  }
}

// References the current redex.
#define RDX (*static_cast<this_type*>(g_redex))

// References the current inductive node.
#define IND (ind<inductive_type>())

// Constructs a new node.  If using garbage collection, use LockedPtr to unlock
// the node when done.
#if SPRITE_GC
#define NODE(type, ...) LockedPtr(new type(__VA_ARGS__))
#else
#define NODE(type, ...) new type(__VA_ARGS__)
#endif

// Constructs a new node that is never reclaimed.
#define STATIC_NODE(name, type, ...) NodePtr name = new type(__VA_ARGS__)

// Performs a rewrite at the current redex.
#define REWRITE(type, ...) rewrite<type>(__VA_ARGS__);

// Performs application to a partially-bound call.
#if SPRITE_GC
#define APPLY(func,arg) LockedPtr(sprite::apply(func,arg))
#else
#define APPLY(func,arg) sprite::apply(func,arg)
#endif

// Performs a conditional rewrite at the current redex.  expr must be an
// expression that reduces to a Boolean result.
#define COND(expr,true_,false_)                  \
    if(cond(new expr)) { true_ } else { false_ } \
  /**/

// Handles an exempt node.  For convenience, this simply expands to a leaf that
// rewrite to fail.
#define DT_EXEMPT (DT_LEAF, REWRITE(FailNode))

// ----

// Defines a constructor node.
#define CONSTRUCTOR(ident, label, arity, id)                  \
    struct ident : Node                                       \
    {                                                         \
      SPRITE_NODE_PREAMBLE(ident, ident, label, arity, id)    \
      virtual void N() { BOOST_PP_REPEAT(arity,CTOR_I,) }     \
      virtual void H() {}                                     \
    };                                                        \
  /**/
// Note: this macro is also used in partial.hpp.
#define CTOR_I(z,n,_) (*this)[n]->N();

// ----

// Defines an operation node.
#define OPERATION(ident, label, arity, dtree)                 \
    struct ident : Node                                       \
    {                                                         \
      SPRITE_NODE_PREAMBLE(ident, ident, label, arity, OPER)  \
      virtual void N() { g_redex = this; dt(); N(); }         \
      virtual void H() { g_redex = this; dt(); H(); }         \
    private:                                                  \
      __attribute__((noinline)) DEFINITIONAL_TREE dtree       \
    };                                                        \
  /**/

// Performs a virtual call through a vtable.
#define TABLE_CALL(pos, table)    \
    g_inductive = get(pos);       \
    g_vtable = &table[4];         \
    g_vtable[g_inductive->tag](); \
  /**/

// ----

#define SPRITE_SHOW(z,n,_) std::cout << " "; CHILD(n)->show();

// Declares the constructor and children.
#define SPRITE_NODE_PREAMBLE(clsname, complete, label, arity_, tag)             \
    BOOST_PP_IF(                                                                \
        BOOST_PP_LESS(arity_, SPRITE_INPLACE_BOUND)                             \
      , SPRITE_NODE_PREAMBLE_I(IP, clsname, complete, arity_, tag)              \
      , SPRITE_NODE_PREAMBLE_I(NIP, clsname, complete, arity_, tag)             \
      )                                                                         \
    typedef clsname this_type;                                                  \
    typedef mpl::size_t<arity_> arity_value;                                    \
    virtual char const * name() { return label; }                               \
    virtual void show()                                                         \
    {                                                                           \
      if(arity_ > 0) std::cout << "(";                                          \
      std::cout << name();                                                      \
      BOOST_PP_REPEAT(arity_,SPRITE_SHOW,)                                      \
      if(arity_ > 0) std::cout << ")";                                          \
    }                                                                           \
    virtual size_t arity() const { return arity_value::value; }                 \
    /* Declare operator[] and at() if arity is not zero. */                     \
    BOOST_PP_EXPR_IF(arity_                                                     \
      , inline NodePtr & operator[](size_t i)                                   \
          { assert(i<arity_); return CHILD(i); }                                \
        virtual NodePtr & at(size_t i) { return (*this)[i]; }                   \
      )                                                                         \
    /* Declare begin and end. */                                                \
    BOOST_PP_EXPR_IF(SPRITE_GC                                                  \
      , BOOST_PP_IF(arity_                                                      \
          , virtual iterator begin()                                            \
              { return reinterpret_cast<iterator>(&this->args[0]); }            \
            virtual iterator end()                                              \
              { return reinterpret_cast<iterator>(&this->args[arity_]); }       \
            virtual const_iterator begin() const                                \
              { return reinterpret_cast<const_iterator>(&this->args[0]); }      \
            virtual const_iterator end() const                                  \
              { return reinterpret_cast<const_iterator>(&this->args[arity_]); } \
          , virtual iterator begin() { return iterator(); }                     \
            virtual iterator end() { return iterator(); }                       \
            virtual const_iterator begin() const { return const_iterator(); }   \
            virtual const_iterator end() const { return const_iterator(); }     \
          )                                                                     \
      )

  /**/

// Manage in-place child lists.
#define SPRITE_ALLOC_IP(arity)
#define SPRITE_DEALLOC_IP(arity)
#define SPRITE_DECL_IP(arity) void * args[arity];
#define SPRITE_INIT_IP(z,n,_) new(&args[n]) NodePtr(arg##n);
#define SPRITE_DEL_IP(z,n,_) CHILD(n).~NodePtr();

// Manage not-in-place child lists.
#define SPRITE_ALLOC_NIP(arity)                                       \
    args = reinterpret_cast<void**>(child_allocator[arity].malloc()); \
  /**/
#define SPRITE_DEALLOC_NIP(arity) child_allocator[arity].free(args);
#define SPRITE_DECL_NIP(arity) void ** args;
#define SPRITE_INIT_NIP(z,n,_) new(&args[n]) NodePtr(arg##n);
#define SPRITE_DEL_NIP(z,n,_) CHILD(n).~NodePtr();

// Expands to the constructor, descructor and data declarations.
// Relies on the following five implementation macros:
//     SPRITE_ALLOC_##mode
//         Expands to a statement that dynamic allocates the child node list.
//     SPRITE_DEALLOC_##mode  
//         Expands to a statement that dynamic deallocates the child node list.
//     SPRITE_DECL_##mode  
//         Expands to the class-level declaration of the child nodes.
//     SPRITE_INIT_##mode  
//         Expands to a statement that initializes one child node.
//     SPRITE_DEL_##mode  
//         Expands to a statement that destroys one child node.
#define SPRITE_NODE_PREAMBLE_I(mode, clsname, complete, arity, tag)      \
    /* Declare the constructor. */                                       \
    clsname(BOOST_PP_ENUM_PARAMS(arity, NodePtr const & arg))            \
      : Node(tag)                                                        \
    {                                                                    \
      SPRITE_ALLOC_##mode(arity)                                         \
      BOOST_PP_REPEAT(arity,SPRITE_INIT_##mode,)                         \
    }                                                                    \
    /* Expands to nothing if arity is zero, or another constructor (from \
     * Partial) otherwise.                                               \
     */                                                                  \
    BOOST_PP_CAT(SPRITE_NODE_PREAMBLE_II_, BOOST_PP_BOOL(arity))         \
        (mode,clsname,complete,arity,tag)                                \
                                                                         \
    BOOST_PP_EXPR_IF(SPRITE_USE_DTOR                                     \
      , virtual ~clsname()                                               \
        {                                                                \
          BOOST_PP_REPEAT(arity,SPRITE_DEL_##mode,)                      \
          SPRITE_DEALLOC_##mode(arity)                                   \
        }                                                                \
      )                                                                  \
    /* Declare the child node storage */                                 \
    SPRITE_DECL_##mode(arity)                                            \
  /**/


/// Adds construction from a partial application (or nothing).
#define SPRITE_NODE_PREAMBLE_II_0(mode, clsname, complete, arity, tag)
#define SPRITE_NODE_PREAMBLE_II_1(mode, clsname, complete, arity, tag) \
    /* Construct from a partial application plus additional args. */   \
    clsname(                                                           \
        Partial<complete,BOOST_PP_DEC(arity)> * part                   \
      , NodePtr const & BOOST_PP_CAT(arg,BOOST_PP_DEC(arity))          \
      )                                                                \
      : Node(tag)                                                      \
    {                                                                  \
      SPRITE_ALLOC_##mode(arity)                                       \
      copy_successors(this, part);                                     \
      BOOST_PP_REPEAT_FROM_TO(                                         \
          BOOST_PP_SUB(arity,1),arity,SPRITE_INIT_##mode,              \
        )                                                              \
    }                                                                  \
  /**/

/**
 * @brief Handles casting tricks.  Gets the nth child of the node when this is
 * in scope.
 */
#define CHILD(n) (*reinterpret_cast<NodePtr *>(&this->args[n]))

namespace sprite
{
  #define F(z,n,_)                                                            \
      template<typename CompleteType>                                         \
      struct Partial<CompleteType,n> : Node                                   \
      {                                                                       \
        SPRITE_NODE_PREAMBLE(Partial, CompleteType, ("Partial_" #n), n, CTOR) \
        virtual void N() { BOOST_PP_REPEAT(n,CTOR_I,) }                       \
        virtual void H() {}                                                   \
        virtual Node * apply(Node * arg)                                      \
        {                                                                     \
          typedef typename                                                    \
              mpl::if_c<                                                      \
                  n+1 == CompleteType::arity_value::value                     \
                , CompleteType                                                \
                , Partial<CompleteType, n+1>                                  \
                >::type                                                       \
            next_type;                                                        \
          next_type * next = new next_type(this, arg);                        \
          return next;                                                        \
        }                                                                     \
      };                                                                      \
    /**/
  
  // Declare partial specializations of Partial (i.e., partial application) where
  // NumBound == 0,1,...SPRITE_ARITY_BOUND-2.
  BOOST_PP_REPEAT(BOOST_PP_DEC(SPRITE_ARITY_BOUND), F, )
  #undef F

  // A holder for a full-sized allocation unit.  A range of NodeH's will have a
  // stride of NODE_BYTES bytes.
  CONSTRUCTOR(NodeH, "NodeH", BOOST_PP_DEC(SPRITE_INPLACE_BOUND), 0)
}

// The implementation of NodePool must come after Node is fully defined.
#include "sprite/detail/memory_impl.hpp"

