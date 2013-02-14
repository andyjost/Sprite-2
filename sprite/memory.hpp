/**
 * @file
 * @brief Sprite components related to memory management (e.g., the memory pool
 * implementation).
 */
#pragma once
#include "sprite/common.hpp"
#include <boost/pool/pool.hpp>
#include "sprite/detail/global_pool.hpp"
#include <stack>

namespace sprite
{
  /**
   * @brief Initializes the memory pools.
   *
   * Due to the so-called static initialization order fiasco, we need a special
   * object to manage initialization of the memory pools.  This object ensures
   * that every compilation unit including this file initializes the memory
   * pools before using them.
   */
  struct PoolInitializer { PoolInitializer(); } _pool_initializer;

  #if SPRITE_GC
    /**
     * The mark is used by the collector for telling which nodes are in use.
     * Whenever a node is allocated, its mark is set to the current value of
     * g_mark.  At the start of collection, the mark is flipped (it is
     * always either 0 or 1) and then reachable nodes are marked with the new
     * value.  During the sweep phase, any node with the old mark value is
     * reclaimed.  Since the collector only runs when the free list is empty,
     * when collection occurs, every node either was in use during the previous
     * collection, or has since been allocated exactly once.  This algorithm
     * will, therefore, never accidentally think an unused node is in use.
     */
    extern mark_type g_mark;

    // A mark used to indicate a node may not be reachable from the root, but
    // nevertheless should not be reclaimed.
    static const mark_type LOCKED_MARK = 2;

    // Used when monitoring GC allocations.
    struct MonitorAllocator
    {
      typedef std::size_t    size_type;
      typedef std::ptrdiff_t difference_type;
      static char * malloc(size_type const sz)
      {
        float const n = sz / NODE_BYTES;
        std::cout << "Request for a block of " << sz << " bytes (" << n << " nodes)." << std::endl;
        char * p = new char[sz];
        std::cout
            << "block-alloc(("
            << reinterpret_cast<void *>(p) << ","
            << reinterpret_cast<void *>((p+sz))
            << "))" << std::endl
          ;
        return p;
      }
      static void free(char * const p)
      {
        std::cout << "block-free " << reinterpret_cast<void *>(p) << std::endl;
        delete[] p;
      }
    };

    /**
     * @brief A memory pool with garbage collection.
     *
     * Extends Boost.Pool (specifically for sprite::Node) with an additional
     * member collect, which performs garbage collection through a mark-sweep
     * algorithm.
     */
    template<typename UserAllocator = boost::default_user_allocator_new_delete>
    struct NodePool : public boost::pool<UserAllocator>
    {
      /// The base pool type.
      typedef boost::pool<UserAllocator> base_type;

      /// The size type for the base pool.
      typedef typename base_type::size_type size_type;

      explicit NodePool(
          size_type nrequested_size
        , size_type nnext_size = 32
        , size_type nmax_size = 0
        )
      : base_type(nrequested_size, nnext_size, nmax_size)
      {
        assert(nnext_size > 0);
      }

      void * malloc BOOST_PREVENT_MACRO_SUBSTITUTION()
      {
        if (!this->store().empty())
          return (this->store().malloc)();
        return malloc_need_collect();
      }
    private:
      // Run the collector.  Returns the number of chunks freed and a pointer
      // to the first chunk freed.
      std::pair<size_type,void*> collect();

      // Perform collection and then malloc a chunk.  If too few nodes are
      // reclaimed, a new block will be allocated.
      void * malloc_need_collect();
    };

    // Node pool using collection.
    #ifdef TRACEGC
    typedef NodePool<MonitorAllocator> node_pool_type;
    #else
      #ifdef SPRITE_NO_REGISTER_FREE_LIST
        typedef NodePool<> node_pool_type;
      #else
        typedef NodePool<GlobalAllocator> node_pool_type;
      #endif
    #endif
  #else

    // Node pool without collection.
    #ifdef SPRITE_NO_REGISTER_FREE_LIST
      typedef boost::pool<> node_pool_type;
    #else
      typedef boost::pool<GlobalAllocator> node_pool_type;
    #endif
  #endif

  /// The pool for allocating new nodes.
  extern node_pool_type * node_allocator;

  /// Used to allocate nodes during the static initialization phase.
  extern boost::pool<> * static_node_allocator;

  /// An array of pools for child lists of length 1,2,...,n.
  extern boost::pool<> * child_allocator;
}
