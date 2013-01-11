/**
 * @file
 * @brief Sprite components related to the evaluation strategy.
 */
#pragma once
#include "sprite/node.hpp"
#include "sprite/builtins.hpp"

namespace sprite
{
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

  inline bool cond(NodePtr const & expr)
  {
    Node * save = g_redex;
    expr->N();
    g_redex = save;
    return is_true(expr);
  }
}
