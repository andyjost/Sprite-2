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
  boost::pool<> * child_allocator;
}

__attribute__((noinline)) void show()
{
  sprite::root->show();
  std::cout << std::endl;
}

