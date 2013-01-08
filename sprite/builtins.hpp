/**
 * @file
 * @brief Built-in node definitions.
 */
#pragma once
#include "sprite/node.hpp"
#include "sprite/maclib.hpp"

namespace sprite
{
  // ==== Int ====
  struct IntNode : Node
  {
    int64 m_value;
    IntNode(int64 value_=0) : Node(CTOR), m_value(value_) {}
    virtual ~IntNode() {}
    virtual char const * name() { return "IntNode"; }
    virtual void show() { std::cout << m_value; }
    virtual void N() {}
    virtual void H() {}
    virtual int64 value() { return m_value; }
    virtual size_t arity() const { return 0; }
    #if SPRITE_GC
      virtual iterator begin() { return iterator(); }
      virtual iterator end() { return iterator(); }
      virtual const_iterator begin() const { return const_iterator(); }
      virtual const_iterator end() const { return const_iterator(); }
    #endif
  };

  // ==== Bool ====
  struct BoolNode;
  bool check_bool(NodePtr const & x);

  inline bool is_false(NodePtr const & x)
  {
    assert(check_bool(x));
    return x->tag == CTOR;
  }

  inline bool is_true(NodePtr const & x)
  {
    assert(check_bool(x));
    return x->tag == (CTOR+1);
  }

  struct BoolNode : Node
  {
    BoolNode(bool value) : Node(value ? 1 : 0) { assert(CTOR == 0); }
    virtual ~BoolNode() {}
    virtual char const * name()
      { return is_false(this) ? "False" : "True"; }
    virtual void show() { std::cout << this->name(); }
    virtual void N() {}
    virtual void H() {}
    virtual size_t arity() const { return 0; }
    #if SPRITE_GC
      virtual iterator begin() { return iterator(); }
      virtual iterator end() { return iterator(); }
      virtual const_iterator begin() const { return const_iterator(); }
      virtual const_iterator end() const { return const_iterator(); }
    #endif
  };

  inline bool check_bool(NodePtr const & x)
    { return dynamic_cast<BoolNode *>(get(x)); }

  // ==== Fwd ====
  struct FwdNode : Node
  {
    NodePtr dest;
    FwdNode(NodePtr const & dest_) : Node(FWD), dest(dest_) {}
    virtual ~FwdNode() {}
    virtual char const * name() { return "FwdNode"; }
    virtual void show() { dest->show(); }
    virtual void N() { dest->N(); }
    virtual void H() { dest->H(); }
    virtual int64 value() { return dest->value(); }
    virtual size_t arity() const { return 0; }
    #if SPRITE_GC
      virtual iterator begin()
        { return reinterpret_cast<iterator>(&dest); }
      virtual iterator end() { return begin() + 1; }
      virtual const_iterator begin() const
        { return reinterpret_cast<const_iterator>(&dest); }
      virtual const_iterator end() const { return begin() + 1; }
    #endif
  };

  // ==== Math Operations ====
  #define SPRITE_MATH_DECL(name, op)                                    \
      struct name : Node                                                \
      {                                                                 \
        SPRITE_NODE_PREAMBLE(name, #op, 2, OPER)                        \
        virtual void N() { H(); }                                       \
        virtual void H()                                                \
        {                                                               \
          (*this)[0]->H(); (*this)[1]->H();                             \
          g_redex = this;                                               \
          rewrite<IntNode>((*this)[0]->value() op (*this)[1]->value()); \
        }                                                               \
      };                                                                \
    /**/
  
  SPRITE_MATH_DECL(AddNode, +)
  SPRITE_MATH_DECL(SubNode, -)
  SPRITE_MATH_DECL(MulNode, *)
  SPRITE_MATH_DECL(DivNode, /)
  SPRITE_MATH_DECL(ModNode, %)

  // ==== Comparison Operations ====
  #define SPRITE_CMP_DECL(name, op)                                      \
      struct name : Node                                                 \
      {                                                                  \
        SPRITE_NODE_PREAMBLE(name, #op, 2, OPER)                         \
        virtual void N() { H(); }                                        \
        virtual void H()                                                 \
        {                                                                \
          (*this)[0]->H(); (*this)[1]->H();                              \
          g_redex = this;                                                \
          rewrite<BoolNode>((*this)[0]->value() op (*this)[1]->value()); \
        }                                                                \
      };                                                                 \
    /**/
  SPRITE_CMP_DECL(LtNode, <)
  SPRITE_CMP_DECL(GtNode, >)
  SPRITE_CMP_DECL(LeNode, <=)
  SPRITE_CMP_DECL(GeNode, >=)
  SPRITE_CMP_DECL(EqNode, ==)
  SPRITE_CMP_DECL(NeNode, !=)

}
