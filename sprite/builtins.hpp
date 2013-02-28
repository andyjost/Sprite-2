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
    virtual Node * apply_(Node * arg) { return dest->apply_(arg); }
    virtual NodePtr & at(size_t i) { return dest->at(i); }
    #if SPRITE_GC
      virtual iterator begin()
        { return reinterpret_cast<iterator>(&dest); }
      virtual iterator end() { return begin() + 1; }
      virtual const_iterator begin() const
        { return reinterpret_cast<const_iterator>(&dest); }
      virtual const_iterator end() const { return begin() + 1; }
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

  namespace lib { using sprite::IntNode; }

  inline int64 Node::fastvalue()
  {
    #ifdef NDEBUG
      return static_cast<IntNode *>(this)->m_value;
    #else
      return this->value();
    #endif
  }

  // This type is manually-defined (i.e., it is not passed to TYPE).  It can be
  // used with DT_BRANCH to reduce an integer expression to HNF.  The value
  // method can subsequently be called to unbox the value.
  #define SPRITE_LIB_UnboxedInt (lib, ((IntNode, "Int", 0)))

  // Interactions directly with unboxed integers.
  inline int64 unbox(NodePtr & x) { x->N(); return x->value(); }
  inline NodePtr box(int64 x) { return NODE(IntNode, x); }


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
    return x->tag == CTOR+1;
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
    virtual int64 value() { return this->tag; }
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

  namespace lib { using sprite::BoolNode; }

  // This type is manually-defined (i.e., it is not passed to TYPE).  Both
  // branches are the same type (BoolNode) but have different values.
  #define SPRITE_LIB_Bool                                    \
      (lib, ((BoolNode, "False", 0))((BoolNode, "True", 0))) \
    /**/
}
