#pragma once
#include "sprite/sprite.hpp"
#include "sprite/lib.hpp"


#define SPRITE_TEST_MAX_LIST_LENGTH 10

using namespace sprite;

namespace sprite { namespace test
{
  namespace detail
  {
    // equal_list [] [] = True
    // equal_list a:as b:bs = if a == b then equal_list as bs else False
    OPERATION(equal_list, "equal_list", 2
      , (DT_BRANCH, RDX[0], SPRITE_LIB_List
          , (DT_BRANCH, RDX[1], SPRITE_LIB_List
              , (DT_LEAF, REWRITE(BoolNode, true))
              , DT_EXEMPT
              )
          , (DT_BRANCH, RDX[1], SPRITE_LIB_List
              , DT_EXEMPT
              , (DT_LEAF
                  , IF(
                        NODE(EqNode, RDX[0]->at(0), IND[0])
                      , THEN(equal_list, RDX[0]->at(1), IND[1])
                      , ELSE(BoolNode, false)
                      )
                  )
              )
          )
      )

    // build_list converts a function call into a Curry list.
    // E.g., build_list(1,2,3,4) -> [1,2,3,4]
    inline NodePtr cons(int64 h, NodePtr t)
      { return NODE(lib::Cons, NODE(IntNode, h), t); }

    inline NodePtr cons(bool h, NodePtr t)
      { return NODE(lib::Cons, NODE(BoolNode, h), t); }

    #define F(z,n,m)                                                          \
        list = cons(BOOST_PP_CAT(arg,BOOST_PP_DEC(BOOST_PP_SUB(m,n))), list); \
      /**/
    #define G(z,n,_)                                           \
        NodePtr build_list(BOOST_PP_ENUM_PARAMS(n, int64 arg)) \
        {                                                      \
          NodePtr list = lib::nil;                             \
          BOOST_PP_REPEAT_##z(n,F,n)                           \
          return list;                                         \
        }                                                      \
      /**/
    BOOST_PP_REPEAT(SPRITE_TEST_MAX_LIST_LENGTH,G,)
    #undef F
    #undef G
  }

  // Evaluate a goal expression.
  inline NodePtr eval(NodePtr const & expr, bool show=false)
  {
    root = expr;
    root->N();
    if(show)
    {
      root->show();
      std::cout << std::endl;
    }
    return root;
  }

  bool is_failure(NodePtr const & p)
  {
    switch(p->tag)
    {
      case FAIL: return true;
      case FWD: return is_failure((reinterpret_cast<FwdNode *>(get(p)))->dest);
      default: return false;
    }
  }

  // Execute a goal of the given type, and provide an interface to the result.
  template<typename Root> struct Exec
  {
    Exec(bool show=false) : m_expr(eval(NODE(Root), show)) {}

    bool fails() { return is_failure(m_expr); }
    
    bool is_int(int64 x) { return m_expr->value() == x; }

    // True if the expression equals the specified list of integers.
    #define F(z,n,_)                                                      \
        BOOST_PP_EXPR_IF(n, template<typename T>)                         \
        bool is_list(BOOST_PP_ENUM_PARAMS(n,T arg))                       \
        {                                                                 \
          NodePtr list = detail::build_list(BOOST_PP_ENUM_PARAMS(n,arg)); \
          NodePtr result = eval(NODE(detail::equal_list, m_expr, list));  \
          return result->value();                                         \
        }                                                                 \
      /**/
    BOOST_PP_REPEAT(SPRITE_TEST_MAX_LIST_LENGTH,F,)
    #undef F
  private:
    NodePtr m_expr;
  };

}}

using namespace sprite::test;
