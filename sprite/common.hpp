/**
 * @file
 * @brief Common definitions for Sprite.
 */
#pragma once

// The maximum arity of any node, plus one.
#ifndef SPRITE_ARITY_BOUND
#define SPRITE_ARITY_BOUND 50
#endif

// The maximum number of children stored within a node, plus one.
#ifndef SPRITE_INPLACE_BOUND
#define SPRITE_INPLACE_BOUND 3
#endif

// The garbage collection mode is controlled by this variable.
//   0: use reference counting
//   1: use garbage collection
//   2: use garbage collection; cross check with reference counting
#ifndef SPRITE_GC_MODE
#  define SPRITE_GC_MODE 1
#endif

// Set variables based on SPRITE_GC_MODE.
#if SPRITE_GC_MODE == 0
#  define SPRITE_REFCNT 1
#  define SPRITE_GC 0
#elif SPRITE_GC_MODE == 1
#  define SPRITE_REFCNT 0
#  define SPRITE_GC 1
#elif SPRITE_GC_MODE == 2
#  define SPRITE_REFCNT 1
#  define SPRITE_GC 1
#endif

// The execution strategy is controlled by this variable.
//   0: use the "function-table" scheme
//   1: use the "label-table" scheme
#ifndef SPRITE_EXEC_MODE
#  define SPRITE_EXEC_MODE 0
#endif

#if SPRITE_EXEC_MODE == 0
#  define SPRITE_IS_FT 1
#  define SPRITE_IS_LT 0
#elif SPRITE_EXEC_MODE == 1
#  define SPRITE_IS_FT 0
#  define SPRITE_IS_LT 1
#endif

// The destructor is not used in GC mode unless TRACEALLOC was set.
#if !defined SPRITE_USE_DTOR
#  if !SPRITE_GC || TRACEALLOC
#    define SPRITE_USE_DTOR 1
#  else
#    define SPRITE_USE_DTOR 0
#  endif
#endif

// Set variables related to step-counting instrumentation.
#ifdef SPRITE_INSTRUMENT_STEPCOUNTERS
#  define SPRITE_COUNT_REWRITE sprite::g_stepcnt[sprite::N_REWRITE]++;
#  define SPRITE_COUNT_ALLOC sprite::g_stepcnt[sprite::N_ALLOC]++;
#  define SPRITE_COUNT_PATTERN_MATCH sprite::g_stepcnt[sprite::N_PATTERN_MATCH]++;
#else
#  define SPRITE_COUNT_REWRITE
#  define SPRITE_COUNT_ALLOC
#  define SPRITE_COUNT_PATTERN_MATCH
#endif

// Global register variables must come before all function defs.
namespace sprite
{
  struct Node;
  typedef void (*procedure)();

  #if SPRITE_REFCNT
  struct NodePtr;
  #else
  typedef Node * NodePtr;
  #endif

  // Configure global register variables ("dt" mode only, except for the free
  // list).
  //
  // Note: According to the x86_64 ABI:
  //
  //     "Registers %rbp, %rbx and %r12 through %r15 belong to the calling
  //     function and the called function is required to preserve their
  //     values".
  //
  // There is no need to recompile shared objects when using these global
  // registers.

  #if SPRITE_IS_FT
  /// Global pointer to the current vtable.
  register procedure * g_vtable asm ("r15");
  /// Global pointer to the current redex.
  register Node * g_redex asm ("r14");
  /// Global pointer to the current inductive node.
  register Node * g_inductive asm ("r13");
  #endif

  #ifdef SPRITE_NO_REGISTER_FREE_LIST
  void * g_free_list;
  #else
  /// Global pointer to the head of the free list for new nodes.
  register void * g_free_list asm ("r12");
  #endif

  #ifdef SPRITE_INSTRUMENT_STEPCOUNTERS
  enum CounterLabels { N_REWRITE = 0, N_ALLOC = 1, N_PATTERN_MATCH = 2 };
  extern unsigned long long g_stepcnt[3];
  #endif

  // In instance of this object must be added as the very first line of main, to perfom
  // system initialization and shutdown.
  struct SystemInitializer
  {
    SystemInitializer();
    ~SystemInitializer();
  };
}

#include <boost/foreach.hpp>
#include <boost/integer.hpp>  
#include <boost/preprocessor.hpp>
#include <boost/static_assert.hpp>
#include <cassert>
#include <exception>
#include <iostream>
#include <string>

namespace sprite
{
  typedef boost::uint_t<32>::exact uint32;
  typedef boost::int_t<64>::exact int64;

  // Note: The constructors for a given type are numbered CTOR, CTOR+1, ...
  enum Tag { FAIL= -4, FWD= -3, CHOICE= -2, OPER= -1, CTOR=0 };
  typedef signed char tag_type;
  typedef uint32 refcnt_type;
  typedef unsigned char mark_type;
  
  struct RuntimeError : std::exception
  {
    RuntimeError(std::string const & msg = std::string())
      : m_msg("Runtime error: " + msg)
    {}
    virtual char const * what() const throw() { return m_msg.c_str(); }
    virtual ~RuntimeError() throw() {}
  private:
    std::string m_msg;
  };

  // Indicates an eager evaluation produced a failure.
  struct ComputationFailed {};

  // The node requires space for 2 pointers (vptr + tag fields) plus
  // extra to hold the children.  The extra space must be at least one
  // pointer (to hold a pointer to the list of children), and must be
  // large enough to hold all of the inplace children.
  size_t const NODE_BYTES =
      (2 + BOOST_PP_MAX(BOOST_PP_DEC(SPRITE_INPLACE_BOUND), 1))
    * sizeof(void *);
}

