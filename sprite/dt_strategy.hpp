/**
 * @file
 * @brief Defines the macros for the "dt" execution strategy.
 *
 * Defines RDX, IND, REWRITE, CONSTRUCTOR, OPERATION.
 */

#pragma once

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

// ----

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

