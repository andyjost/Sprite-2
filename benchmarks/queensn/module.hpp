#pragma once
#include "sprite/sprite.hpp"
#include "sprite/lib/int.hpp"
#include "sprite/lib/list.hpp"
#include "sprite/lib/bool.hpp"

namespace sprite { namespace module { namespace queensn
{
  /*
  -- ORIGINAL

  nsoln nq = length (gen nq)
      where
      safe :: Int -> Int -> [Int] -> Bool
      safe x d []    = True
      safe x d (q:l) = x /= q && x /= q+d && x /= q-d && safe x (d+1) l
    
      gen :: Int -> [[Int]]
      gen 0 = [[]]
      gen n = [ (q:b) | b <- gen (n-1), q <- [1..nq], safe q 1 b]

  -- SYNTACTICALLY DESUGARED

  nsoln nq = length (gen nq nq)
  gen r l = if l==0 then [[]]
     else filter lambda_0 (concatMap (lambda_1 r) (gen r (l-1)))

  lambda_0 (q:b) = safe q 1 b
  lambda_1 r b = (map (lambda_2 b) [1..r])
  lambda_2 b q = q:b

  safe _ _ []    = True
  safe x d (q:l) = x /= q && x /= q+d && x /= q-d && safe x (d+1) l

  */

  using namespace sprite;
  using lib::constants::i0;
  using lib::constants::i1;
  using lib::constants::i10;

  namespace local
  {
    struct enumFromTo;

    namespace eager
    {
      inline NodePtr enumFromTo(NodePtr n, NodePtr m)
      {
        int64 const n_ = unbox(n);
        int64 const m_ = unbox(m);
        if(n_>m_)
          return lib::nil;
        else
        {
          return NODE(
              lib::Cons, n, NODE(local::enumFromTo, NODE(IntNode, n_+1), m)
            );
        }
      }
    }

    // enumFromTo n m = if n>m then [] else n : enumFromTo (n+1) m
    // denoted as [n..m]
    OPERATION(enumFromTo, "enumFromTo", 2
      , (DT_LEAF
          , NodePtr result;

            BEGIN_EAGER
              result = eager::enumFromTo(RDX[0], RDX[1]);
            END_EAGER

            REWRITE(FwdNode, result);
          )
      )
  }

  namespace eager
  {
    inline bool safe(int64 x, int64 d, NodePtr ql)
    {
      static void * table[] = { &&nil, &&cons };

      ql = H(ql);
      assert(ql->tag >= CTOR);
      goto *table[ql->tag];

    nil:
      return true;

    cons:
      int64 const q = unbox(ql->at(0));
      return (
          x != q && x != (q+d) && x != (q-d) && safe(x, d+1, ql->at(1))
        );
    }

    inline bool safe(NodePtr x, NodePtr d, NodePtr ql)
    {
      static void * table[] = { &&nil, &&cons };

      ql = H(ql);
      assert(ql->tag >= CTOR);
      goto *table[ql->tag];

    nil:
      return true;

    cons:
      int64 const x_ = unbox(x);
      int64 const d_ = unbox(d);
      return safe(x_, d_, ql);
    }
  }

  // safe _ _ []    = True
  // safe x d (q:l) = x /= q && x /= q+d && x /= q-d && safe x (d+1) l
  OPERATION(safe, "safe", 3
    , (DT_LEAF, 
          bool result;

          BEGIN_EAGER
            result = eager::safe(RDX[0], RDX[1], RDX[2]);
          END_EAGER

          REWRITE(BoolNode, result);
        )
    )

  // lambda_2 b q = q:b
  OPERATION(lambda_2, "lambda_2", 2
    , (DT_LEAF, REWRITE(lib::Cons, RDX[1], RDX[0]))
    )


  // lambda_1 r b = (map (lambda_2 b) [1..r])
  OPERATION(lambda_1, "lambda_1", 2
    , (DT_LEAF
        , REWRITE(lib::map
            , NODE(PARTIAL(lambda_2,1), RDX[1])
            , NODE(local::enumFromTo, i1, RDX[0])
            )
        )
    )


  // lambda_0 (q:b) = safe q 1 b
  OPERATION(lambda_0, "lambda_0", 1
    , (DT_BRANCH, RDX[0], SPRITE_LIB_List
       , DT_EXEMPT
       , (DT_LEAF, REWRITE(safe, IND[0], i1, IND[1]))
       )
    )

  struct gen;

  namespace eager
  {
    NodePtr gen(NodePtr r, NodePtr l)
    {
      int64 const l_ = unbox(l);
      if(l_==0)
        return NODE(lib::Cons, lib::nil, lib::nil);
      else
      {
        return NODE(
            lib::filter
          , NODE(PARTIAL(lambda_0,0))
          , NODE(lib::apply
              , NODE(lib::concatMap, NODE(PARTIAL(lambda_1,1), r))
              , NODE(queensn::gen, r, NODE(SubNode, l, i1))
              )
          );
      }
    }
  }

  // gen r l = if l==0 then [[]]
  // else filter lambda_0 (concatMap (lambda_1 r) (gen r (l-1)))
  OPERATION(gen, "gen", 2
    , (DT_LEAF,
          NodePtr result;

          BEGIN_EAGER
            result = eager::gen(RDX[0], RDX[1]);
          END_EAGER

          REWRITE(FwdNode, result);
        )
    )

  OPERATION(nsoln, "nsoln", 1
    , (DT_LEAF, REWRITE(lib::length, NODE(gen, RDX[0], RDX[0])))
    )


  OPERATION(MainNode, "main", 0, (DT_LEAF, REWRITE(nsoln, i10)))
}}}
