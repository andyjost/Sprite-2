/**
 * @file
 * @brief Implements a memory pool that uses a dedicated register for the free
 * list head.
 */
#pragma once
#include "sprite/common.hpp"
#include <boost/pool/pool.hpp>

namespace sprite
{
  template<typename SizeType> struct SizeTypeOverride
  {
    typedef SizeType size_type;

    SizeTypeOverride(size_type size) : m_value(size) {}
    operator size_type () const { return m_value; }
    operator size_type & () { return m_value; }
  private:
    size_type m_value;
  };

  struct GlobalAllocator
    : boost::default_user_allocator_new_delete
  {
    typedef SizeTypeOverride<
        boost::default_user_allocator_new_delete::size_type
      > size_type;
  };

}

namespace boost {

  template <typename SizeType>
  class simple_segregated_storage<sprite::SizeTypeOverride<SizeType> >
  {
    public:
      typedef SizeType size_type;
  
    private:

      void * try_malloc_n(
          void * & start, size_type n, const size_type partition_size)
      {
        void * iter = nextof(start);
        while (--n != 0)
        {
          void * next = nextof(iter);
          if (next != static_cast<char *>(iter) + partition_size)
          {
            // next == 0 (end-of-list) or non-contiguous chunk found
            start = iter;
            return 0;
          }
          iter = next;
        }
        return iter;
      }
  
    protected:
      // Unused, but must be defined to avoid compile errors.  When boost::pool
      // is used as a base class of NodePool, the compiler automatically
      // generates ~pool, which needs first.  We don't really plan to ever call
      // ~pool, though.
      void * first;
  
      // Traverses the free list referred to by "sprite::g_free_list",
      //  and returns the iterator previous to where
      //  "ptr" would go if it was in the free list.
      // Returns 0 if "ptr" would go at the beginning
      //  of the free list (i.e., before "sprite::g_free_list")
      void * find_prev(void * const ptr)
      {
        // Handle border case
        if (sprite::g_free_list == 0 || std::greater<void *>()(sprite::g_free_list, ptr))
          return 0;
      
        void * iter = sprite::g_free_list;
        while (true)
        {
          // if we're about to hit the end or
          //  if we've found where "ptr" goes
          if (nextof(iter) == 0 || std::greater<void *>()(nextof(iter), ptr))
            return iter;
      
          iter = nextof(iter);
        }
      }
  
      static void * & nextof(void * const ptr)
        { return *(static_cast<void **>(ptr)); }
  
    public:
      // Do not set sprite::g_free_list = 0.  That must occur after the
      // program's static initialization phase.
      simple_segregated_storage() {}
  
      void * segregate(
          void * const block,
          const size_type sz,
          const size_type partition_sz,
          void * const end)
      {
        // Get pointer to last valid chunk, preventing overflow on size calculations
        //  The division followed by the multiplication just makes sure that
        //    old == block + partition_sz * i, for some integer i, even if the
        //    block size (sz) is not a multiple of the partition size.
        char * old = static_cast<char *>(block)
            + ((sz - partition_sz) / partition_sz) * partition_sz;
      
        // Set it to point to the end
        nextof(old) = end;
      
        // Handle border case where sz == partition_sz (i.e., we're handling an array
        //  of 1 element)
        if (old == block)
          return block;
      
        // Iterate backwards, building a singly-linked list of pointers
        for (char * iter = old - partition_sz; iter != block;
            old = iter, iter -= partition_sz)
          nextof(iter) = old;
      
        // Point the sprite::g_free_list pointer, too
        nextof(block) = old;
      
        return block;
      }

      void add_block(void * const block,
          const size_type nsz, const size_type npartition_sz)
      {
        // Segregate this block and merge its free list into the
        //  free list referred to by "sprite::g_free_list"
        sprite::g_free_list = segregate(block, nsz, npartition_sz, sprite::g_free_list);
      }

      void add_ordered_block(void * const block,
          const size_type nsz, const size_type npartition_sz)
      {
        throw sprite::RuntimeError("cannot call add_ordered_block from node pool");
        return;
        // // This (slower) version of add_block segregates the
        // //  block and merges its free list into our free list
        // //  in the proper order

        // // Find where "block" would go in the free list
        // void * const loc = find_prev(block);

        // // Place either at beginning or in middle/end
        // if (loc == 0)
        //   add_block(block, nsz, npartition_sz);
        // else
        //   nextof(loc) = segregate(block, nsz, npartition_sz, nextof(loc));
      }

  
      bool empty() const { return (sprite::g_free_list == 0); }
  
      // pre: !empty()
      void * malloc BOOST_PREVENT_MACRO_SUBSTITUTION()
      {
        SPRITE_COUNT_ALLOC
        void * const ret = sprite::g_free_list;
  
        // Increment the "sprite::g_free_list" pointer to point to the next chunk
        sprite::g_free_list = nextof(sprite::g_free_list);
        return ret;
      }
  
      // pre: chunk was previously returned from a malloc() referring to the
      //  same free list
      // post: !empty()
      void free BOOST_PREVENT_MACRO_SUBSTITUTION(void * const chunk)
      {
        nextof(chunk) = sprite::g_free_list;
        sprite::g_free_list = chunk;
      }
  
      void ordered_free(void * const chunk)
      {
        throw sprite::RuntimeError("cannot call ordered_free from node pool");
        return;
        // // This (slower) implementation of 'free' places the memory
        // //  back in the list in its proper order.

        // // Find where "chunk" goes in the free list
        // void * const loc = find_prev(chunk);

        // // Place either at beginning or in middle/end
        // if (loc == 0)
        //   (free)(chunk);
        // else
        // {
        //   nextof(chunk) = nextof(loc);
        //   nextof(loc) = chunk;
        // }
      }
  
      // Note: if you're allocating/deallocating n a lot, you should
      //  be using an ordered pool.
      void * malloc_n(const size_type n, const size_type partition_size)
      {
        throw sprite::RuntimeError("cannot malloc_n from node pool");
        return 0;
        // if(n == 0)
        //   return 0;
        // void * start = &sprite::g_free_list;
        // void * iter;
        // do
        // {
        //   if (nextof(start) == 0)
        //     return 0;
        //   iter = try_malloc_n(start, n, partition_size);
        // } while (iter == 0);
        // void * const ret = nextof(start);
        // nextof(start) = nextof(iter);
        // return ret;
      }
  
      void free_n(void * const chunks, const size_type n,
          const size_type partition_size)
      {
        // if(n != 0)
        //   add_block(chunks, n * partition_size, partition_size);
      }

      void ordered_free_n(void * const chunks, const size_type n,
          const size_type partition_size)
      {
        if(n != 0)
          add_ordered_block(chunks, n * partition_size, partition_size);
      }
  };

}
