/**
 * @file
 * @brief Built-in node definitions.
 */
#pragma once
#include "sprite/node.hpp"
#include "sprite/maclib.hpp"

namespace sprite
{
  // ==== Fail ====
  struct FailNode : Node
  {
    FailNode() : Node(FAIL) {}
    virtual ~FailNode() {}
    static std::string static_name() { return "failure"; }
    virtual std::string name() { return static_name(); }
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

  // ==== Int ====
  struct IntNode : Node
  {
    int64 m_value;
    IntNode(int64 value_=0) : Node(CTOR), m_value(value_) {}
    virtual ~IntNode() {}
    static std::string static_name() { return "Int"; }
    virtual std::string name() { return static_name(); }
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
    static std::string static_name() { return "Bool"; }
    virtual std::string name() { return static_name(); }
    virtual void show() { std::cout << (is_false(this) ? "False" : "True"); }
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
    static std::string static_name() { return "FwdNode"; }
    virtual std::string name() { return static_name(); }
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
  #define SPRITE_BINOP_DECL(name, type, op)                          \
      struct name : Node                                             \
      {                                                              \
        SPRITE_NODE_PREAMBLE(name, name, #op, 2, OPER)               \
        virtual void N() { H(); }                                    \
        virtual void H()                                             \
        {                                                            \
          (*this)[0]->H(); (*this)[1]->H();                          \
          g_redex = this;                                            \
          rewrite<type>((*this)[0]->value() op (*this)[1]->value()); \
        }                                                            \
      };                                                             \
    /**/
  
  SPRITE_BINOP_DECL(AddNode, IntNode, +)
  SPRITE_BINOP_DECL(SubNode, IntNode, -)
  SPRITE_BINOP_DECL(MulNode, IntNode, *)
  SPRITE_BINOP_DECL(DivNode, IntNode, /)
  SPRITE_BINOP_DECL(ModNode, IntNode, %)
  SPRITE_BINOP_DECL(LtNode, BoolNode, <)
  SPRITE_BINOP_DECL(GtNode, BoolNode, >)
  SPRITE_BINOP_DECL(LeNode, BoolNode, <=)
  SPRITE_BINOP_DECL(GeNode, BoolNode, >=)
  SPRITE_BINOP_DECL(EqNode, BoolNode, ==)
  SPRITE_BINOP_DECL(NeNode, BoolNode, !=)
}
