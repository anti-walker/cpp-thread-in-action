#include "spin_lock.h"
#include <thread>


void spin_lock::lock()
{
  while(_lock.test_and_set())
  {
    std::this_thread::yield();
  }
}

void spin_lock::unlock()
{
  _lock.clear();
}
