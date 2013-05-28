/**
 * @file
 * @brief Defines the macros and implementation functions for the "label-table"
 * execution strategy.
 *
 * Defines RDX, IND, REWRITE, CONSTRUCTOR, OPERATION, cond.
 */

#pragma once
#include "sprite/builtins.hpp"

// References the current redex.
#define RDX (*this)

// References the current inductive node.
#define IND (ind<inductive_type>(inductive))

// Performs a rewrite at the current redex.
#define REWRITE(type, ...) rewrite<type>(this ,##__VA_ARGS__);

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
      GENERATE_NH_METHOD(N,dtree)                            \
      GENERATE_NH_METHOD(H,dtree)                            \
    private:                                                 \
      /* Generate the rewrite rules. */                      \
      DEFINITIONAL_TREE(NOTHING, LEAF_RW_DEF, dtree)         \
    };                                                       \
  /**/

// ----

#define GENERATE_NH_METHOD(NH,dtree)                          \
    virtual void NH()                                         \
    {                                                         \
      SPRITE_COUNT_PATTERN_MATCH                              \
      EXPR_IF_BRANCH(Node * inductive;, dtree)                \
      DEFINITIONAL_TREE(BRANCH_DEF, NOTHING, dtree)           \
      DEFINITIONAL_TREE(BRANCH_EXEC, LEAF_EXEC##_##NH, dtree) \
      EXPR_IF_BRANCH(                                         \
          fail: REWRITE(FailNode) return;                     \
          choice: throw RuntimeError("Not implemented");      \
        , dtree                                               \
        )                                                     \
    }                                                         \
  /**/

// Indicates whether the path in A0 is empty.
#define HAS_PATH(A0)                                   \
    BOOST_PP_GREATER(BOOST_PP_SEQ_SIZE(A0_PATH(A0)),1) \
  /**/

// Expands to expr if the path is not empty (else nothing).
#define EXPR_IF_PATH(expr,A0) BOOST_PP_EXPR_IF(HAS_PATH(A0), expr)

// Builds a label if the path is not empty.
#define LABEL_IF_PATH(base,A0) EXPR_IF_PATH(IDENTIFIER(base,A0,):,A0)

#define DT_BRANCH_IS_BRANCH 1
#define DT_LEAF_IS_BRANCH 0

// Takes a pdt specification.  Expands to expr if the head of the pdt is a
// branch, or else nothing.
#define EXPR_IF_BRANCH(expr,dtree)                \
      BOOST_PP_EXPR_IF(                           \
          BOOST_PP_CAT(VA_HEAD dtree, _IS_BRANCH) \
        , expr                                    \
        )                                         \
    /**/

#define BRANCH_DEF(A0,pos,TD,...)                                           \
    static void * IDENTIFIER(utable,A0,)[] =                                \
        { &&fail, &&IDENTIFIER(fwd,A0,), &&choice, &&IDENTIFIER(oper,A0,)   \
          REPEAT(A0)(VA_LEN(__VA_ARGS__),TDB_I,A0)                          \
        };                                                                  \
    static void ** IDENTIFIER(table,A0,) = &IDENTIFIER(utable,A0,)[4];      \
  /**/
#define TDB_I(z,n,A0), &&IDENTIFIER(ctor,A0,_##n)
#define NOTHING(...)

#define BRANCH_EXEC(A0,pos,TD,...)                                \
    LABEL_IF_PATH(ctor,A0)                                        \
    {                                                             \
      typedef A0_CTORNAME(A0) inductive_type;                     \
      inductive = get(pos);                                       \
      goto *IDENTIFIER(table,A0,)[inductive->tag];                \
    }                                                             \
    IDENTIFIER(fwd,A0,):                                          \
      inductive = get((static_cast<FwdNode *>(inductive))->dest); \
      goto *IDENTIFIER(table,A0,)[inductive->tag];                \
    IDENTIFIER(oper,A0,):                                         \
      inductive->H();                                             \
      goto *IDENTIFIER(table,A0,)[inductive->tag];                \
  /**/

#if 0
// Used for dt-style execution.

#define LEAF_EXEC(A0,rule)                             \
    LABEL_IF_PATH(ctor,A0)                             \
    {                                                  \
      typedef A0_CTORNAME(A0) inductive_type;          \
      rule                                             \
      return;                                          \
    }                                                  \
  /**/
#endif


#define LEAF_EXEC_H(A0,rule) LEAF_EXEC(A0,return this->H();)
#define LEAF_EXEC_N(A0,rule) LEAF_EXEC(A0,return this->N();)

#define LEAF_EXEC(A0,tail)                       \
    BOOST_PP_CAT(                                \
        LEAF_EXEC                                \
      , BOOST_PP_IF(HAS_PATH(A0),_PATH,_NO_PATH) \
      )                                          \
    (A0,tail)                                    \
  /**/

// Triggers a conditional rule.  There is an inductive node (and type).
#define LEAF_EXEC_PATH(A0,tail)                        \
    IDENTIFIER(ctor,A0,):                              \
    {                                                  \
      typedef A0_CTORNAME(A0) inductive_type;          \
      IDENTIFIER(rule,A0,)<inductive_type>(inductive); \
      tail                                             \
    }                                                  \
  /**/

// Triggers an unconditional rule.  There is no inductive node (or type).
#define LEAF_EXEC_NO_PATH(A0,tail) IDENTIFIER(rule,A0,)(); tail

// Performs a rewrite step in a separate stack frame.  This is a workaround
// that ensures the compiler will properly use tail recurusion.
#define LEAF_RW_DEF(A0,ruleexpr)                 \
    BOOST_PP_CAT(                                \
        LEAF_RW_DEF                              \
      , BOOST_PP_IF(HAS_PATH(A0),_PATH,_NO_PATH) \
      )                                          \
    (A0,ruleexpr)                                \
  /**/

#define LEAF_RW_DEF_PATH(A0,ruleexpr)                           \
    template<typename inductive_type> __attribute__((noinline)) \
    void IDENTIFIER(rule,A0,)(Node * inductive) { ruleexpr }    \
  /**/

#define LEAF_RW_DEF_NO_PATH(A0,ruleexpr)     \
    __attribute__((noinline))                \
    void IDENTIFIER(rule,A0,)() { ruleexpr } \
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
  template<typename NodeType> inline NodeType & ind(Node * inductive)
    { return *static_cast<NodeType *>(inductive); }

  namespace lib { struct test; }

  inline bool cond(NodePtr const & expr)
  {
    LOCAL_NODE(tmp, lib::test, expr);
    tmp->N();
    return is_true(tmp);
  }
}
