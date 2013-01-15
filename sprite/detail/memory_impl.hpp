/**
 * @file
 * @brief Template implementation for memory.hpp.
 */
#pragma once
#include "sprite/timer.hpp"

namespace sprite
{
  #if SPRITE_GC
    // pre: the free list must be empty.
    // post: the free list may not be empty.
    template<typename UserAllocator>
    std::pair<typename NodePool<UserAllocator>::size_type, void *>
    NodePool<UserAllocator>::collect()
    {
      mark_type const prev_mark = g_mark;
      g_mark = 1 - g_mark;
      assert(g_mark == 0 || g_mark == 1);
  
      #if TRACEGC
        std::cout << "Starting collection with mark = " << (int)g_mark
          << std::endl;
        ticks t0 = getticks();
        ticks t1;
      #endif
  
      // Mark phase.
      std::stack<Node *> roots;
      if(root) roots.push(get(root));
      while(!roots.empty())
      {
        Node * parent = roots.top();
        roots.pop();
        #if TRACEGC > 1
          std::cout << "mark (" << (int)parent->mark << "->" << (int)g_mark
            << ") @" << parent << std::endl;
        #endif

        if(parent->mark != g_mark)
        {
          parent->mark = g_mark;
          for(Node::iterator p = parent->begin(), e = parent->end(); p!=e; ++p)
            roots.push(*p);
        }
      }

      #if TRACEGC
        t1 = getticks();
        std::cout << "Mark phase takes " << (t1-t0) << " ticks." << std::endl;
        t0 = getticks();
      #endif
  
      // Sweep phase.
      #if TRACEGC
        size_t total = 0;
      #endif
      size_t n = 0;
      void * last = 0; // last item in the free list (first chunk freed).
      // An iterator over the memory block list.
      typedef boost::details::PODptr<size_type> blockptr;
      const size_type partition_size = this->alloc_size();
      for(blockptr ptr = this->list; ptr.valid(); ptr = ptr.next())
      {
        #if TRACEGC
          total += (ptr.end() - ptr.begin());
        #endif
        for(char * i = ptr.begin(); i != ptr.end(); i += partition_size)
        {
          Node * const node = reinterpret_cast<Node *>(i);
          // Do not reclaim if mark is the current mark (g_mark), or
          // LOCKED_MARK.  That only leaves the previous mark.
          if(node->mark == prev_mark)
          {
            ++n;
            if(!last) last = node;

            // The destructor is not called here unless TRACEALLOC was set.
            #ifdef SPRITE_USE_DTOR
              node->~Node();
            #endif

            this->free(node);
          }
        }
      }
      #if TRACEGC
        t1 = getticks();
        std::cout << "Sweep phase takes " << (t1-t0) << " ticks." << std::endl;

        total /= partition_size;
        float const pct =  total == 0 ? 0 : 100.0 * n / total;
        std::cout << "Done collecting: freed " << n << " out of "
          << total << " chunks (" << pct << "%)." << std::endl;
      #endif
      return std::make_pair(n,last);
    }
  
    template<typename UserAllocator>
    void * NodePool<UserAllocator>::malloc_need_collect()
    {
      // Run the collector.
      std::pair<size_type,void *> const collected = collect();
      size_type const nfree = collected.first;
      void * last = collected.second;
  
      // If too little space was made, then allocate a new block, too.  If more
      // than 1 part in 32 remains in use after collection, allocate a new block.
      size_type const current_size = this->next_size - this->start_size;
      size_type const threshold = current_size >> 5;
      if(current_size - nfree >= threshold)
      {
        // Save the free list and then set it to empty in this object.
        void * const save_list = this->first;
        this->first = 0;
        assert(!save_list == !last); // both are null or both are non-null
  
        // This call will allocate a new block, since the free list is empty.
        void * ret = base_type::malloc();
        
        // If the old free list was not empty, insert it before the new block.
        if(last)
        {
          this->nextof(last) = this->first;
          this->first = save_list;
        }
        return ret;
      }
      else 
      {
        // Return one of the collected items without allocating a new block.
        assert(!this->store().empty());
        return (this->store().malloc)();
      }
    }
  #endif
}
