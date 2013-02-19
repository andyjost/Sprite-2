/**
 * @file
 * @brief Defines the macros and implementation functions for the
 * "function-table" execution strategy.
 *
 * Defines RDX, IND, REWRITE, CONSTRUCTOR, OPERATION, cond.
 */

#pragma once
#include "sprite/builtins.hpp"

// References the current redex.
#define RDX (*static_cast<this_type*>(g_redex))

// References the current inductive node.
#define IND (ind<inductive_type>())

// Performs a rewrite at the current redex.
#define REWRITE(type, ...) rewrite<type>(__VA_ARGS__);

// ----

// Defines a constructor node.
#define CONSTRUCTOR(ident, label, arity, id)               \
    struct ident : Node                                    \
    {                                                      \
      SPRITE_NODE_PREAMBLE(ident, ident, label, arity, id) \
      virtual void N() { BOOST_PP_REPEAT(arity,CTOR_I,) }  \
      virtual void H() {}                                  \
    };                                                     \
  /**/
// Note: this macro is also used in partial.hpp.
#define CTOR_I(z,n,_) (*this)[n]->N();

// Defines an operation node.
#define OPERATION(ident, label, arity, dtree)                \
    struct ident : Node                                      \
    {                                                        \
      SPRITE_NODE_PREAMBLE(ident, ident, label, arity, OPER) \
      virtual void N() { g_redex = this; dt(); N(); }        \
      virtual void H() { g_redex = this; dt(); H(); }        \
    private:                                                 \
      __attribute__((noinline))                              \
      DEFINITIONAL_TREE(BRANCH,LEAF,dtree)                   \
    };                                                       \
  /**/

// ----

// Handles branches.
#define BRANCH(A0,pos,TD,...)                                               \
    /* The number of constructors in type must match the number of extra */ \
    /* args provided.                                                    */ \
    BOOST_PP_ASSERT(                                                        \
        BOOST_PP_EQUAL(TD_LEN(TD), VA_LEN(__VA_ARGS__))                     \
      )                                                                     \
                                                                            \
    /* Define the function to handle this branch in the DTree. */           \
    static void IDENTIFIER(dt,A0,)()                                        \
    {                                                                       \
      typedef A0_CTORNAME(A0) inductive_type;                               \
      static procedure vtable[] = { &fail, &fwd, &choice, &oper             \
          REPEAT(A0)(VA_LEN(__VA_ARGS__),BRANCH_I,A0)                       \
        };                                                                  \
      g_inductive = get(pos);                                               \
      g_vtable = &vtable[4];                                                \
      g_vtable[g_inductive->tag]();                                         \
    }                                                                       \
  /**/
#define BRANCH_I(z,n,A0), &this_type::IDENTIFIER(dt,A0,_##n)

// Handles leaves.
#define LEAF(A0,rule)                         \
    static void IDENTIFIER(dt,A0,)()          \
    {                                         \
      typedef A0_CTORNAME(A0) inductive_type; \
      rule                                    \
    }                                         \
  /**/

namespace sprite
{
  /** 
   * Gives the default type of the inductive node.  Inside of a scope that
   * performs a rewrite step, this may be redefined.  Then the expression
   * ind<inductive_type>() always gives the inductive node, cast to its correct
   * type.
   */
  typedef Node inductive_type;

  /// Casts the current inductive node to a particular target type.
  template<typename NodeType> inline NodeType & ind()
    { return *static_cast<NodeType *>(g_inductive); }

  // Default H action for failure nodes.
  inline void fail() { REWRITE(FailNode); }
  // Default H action for FWD nodes.
  inline void fwd()
  {
    g_inductive = get((static_cast<FwdNode *>(g_inductive))->dest);
    g_vtable[g_inductive->tag]();
  }
  // Default H action for CHOICE nodes.
  inline void choice() { throw RuntimeError(); }
  // Default H action for OPER nodes.
  inline void oper() { g_inductive->H(); }

  namespace lib { struct test; }

  inline bool cond(NodePtr const & expr)
  {
    Node * redex = g_redex;
    Node * inductive = g_inductive;
    LOCAL_NODE(tmp, lib::test, expr);
    tmp->N();
    g_redex = redex;
    g_inductive = inductive;
    return is_true(tmp);
  }
}
