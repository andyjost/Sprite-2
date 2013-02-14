#include "sprite/sprite.hpp"


namespace sprite
{
  NodePtr root;
  mark_type g_mark;

  PoolInitializer::PoolInitializer()
  {
    g_mark = 0;
    if(!node_allocator)
    {
      // Start with a request for 1024 nodes.
      node_allocator = new node_pool_type(NODE_BYTES, 1024);
    }
    if(!static_node_allocator)
    {
      static_node_allocator = new boost::pool<>(NODE_BYTES, 128);
    }
    if(!child_allocator)
    {
      boost::pool<> * p = reinterpret_cast<boost::pool<> *>(
          new char[sizeof(boost::pool<>) * SPRITE_ARITY_BOUND]
        );
      for(size_t i=SPRITE_INPLACE_BOUND; i<SPRITE_ARITY_BOUND; ++i)
      {
        new(&p[i]) boost::pool<>(i * sizeof(void *));
      }
      child_allocator = p;
    }
  }

  node_pool_type * node_allocator;
  boost::pool<> * static_node_allocator;
  boost::pool<> * child_allocator;

  // This function must be called in main, after static initialization is
  // complete.
  //
  // The g_free_list (a dedicated register) must be initializes after main
  // begins.
  SystemInitializer::SystemInitializer()
  {
    static bool initialized = false;
    if(!initialized)
    {
      initialized = true;
      g_free_list = 0;
    }
  }

  // This function must be called in main, just before static destruction
  // begins.
  //
  // Any global objects with destructors that might call ~Node must be cleared
  // before main returns.  Otherwise, memory will be reclaimed during static
  // desctruction, which would use g_free_list.
  SystemInitializer::~SystemInitializer()
  {
    // Increment the reference count to avoid ever reclaiming these objects.
    #if SPRITE_REFCNT
    Node * p = get(root);
    ++p->refcnt;
    #endif

    root = NodePtr();
  }
}

__attribute__((noinline)) void show()
{
  sprite::root->show();
  std::cout << std::endl;
}

