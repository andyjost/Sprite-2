#pragma once
#include "sprite/sprite.hpp"
#include "sprite/lib/int.hpp"

namespace sprite { namespace module { namespace takn
{
  /*
    tak :: Int -> Int -> Int -> Int
    tak x y z = if x <= y then z
                else tak (tak (x-1) y z)
                         (tak (y-1) z x)
                         (tak (z-1) x y)
    goal1 = tak 27 16 8
    main = do
      print $ goal1
  */

  using namespace sprite;
  using namespace lib::constants;

  // ---- Moved to sprite/builtins.hpp ----
  // inline int64 unbox(NodePtr & x) { x->N(); return x->value(); }
  // inline NodePtr box(int64 x) { return NODE(IntNode, x); }

  // ---- Moved to sprite/ft_strategy.hpp ----
  // struct Scope
  // {
  //   Scope() : m_redex(g_redex) {}
  //   ~Scope() { g_redex = m_redex; }
  // private:
  //   Node * const m_redex;
  // };

  namespace eager
  {
    int64 tak(int64 x, int64 y, int64 z)
    {
      if(x<=y)
        return z;
      else
        return tak(
            tak(x-1, y, z)
          , tak(y-1, z, x)
          , tak(z-1, x, y)
          );
    }

    NodePtr tak(int64 x, int64 y, NodePtr z)
    {
      if(x<=y)
        return z;
      else
      {
        int64 const z_ = unbox(z);
        return box(
            tak(
                tak(x-1, y, z_)
              , tak(y-1, z_, x)
              , tak(z_-1, x, y)
              )
          );
      }
    }
  }

  OPERATION(TakNode, "tak", 3
    , (DT_LEAF,
          NodePtr result;
          {
            Scope();
            result = eager::tak(unbox(RDX[0]), unbox(RDX[1]), RDX[2]);
          }
          REWRITE(FwdNode, result);
        )
    )

  OPERATION(Goal1Node, "goal1", 0 ,(DT_LEAF, REWRITE(TakNode, i27, i16, i8)))
  OPERATION(MainNode, "main", 0, (DT_LEAF, REWRITE(Goal1Node)))
}}}
