#include "thread_safe_stack.hpp"
#include "thread_safe_queue.hpp"
#include "spin_lock.h"
#include <vector>
#include <thread>
#include <iostream>
#include <sstream>

const int LOOP_NUMBER = 1000;
const int PRODUCER_NUMBER = 4;
const int CONSUMER_NUMBER = 3;

template<class Lockable>
void generrate_data(threadsafe_stack<int,Lockable>& s, threadsafe_queue<std::string>& q)
{
  std::string strLog;
  for(int i = 0; i < LOOP_NUMBER; ++i)
  {
    s.push(i);

    std::stringstream ss;
    ss << "thread id: " << std::this_thread::get_id() << " produced: " << i;
    strLog = ss.str();
    q.push(strLog);
  }
}

template<class Lockable>
void consume_data(threadsafe_stack<int,Lockable>& s, threadsafe_queue<std::string>& q, bool& bStopProduced)
{
  std::string strLog;
  int value;
  while(!bStopProduced || !s.empty())
  {
    try
    {
      s.pop(value);

      std::stringstream ss;
      ss << "thread id: " << std::this_thread::get_id() << " consumed: " << value;
      strLog = ss.str();
      q.push(strLog);
    }
    catch(std::exception& e)
    {
      std::stringstream ss;
      ss << "thread id: " <<  std::this_thread::get_id() << " " << e.what() << " while consume";
      strLog = ss.str();
      q.push(strLog);
    }

  }

}

void printLog(threadsafe_queue<std::string>& q, bool& bDone)
{
  std::string strLog;
  while(!bDone || !q.empty())
  {
    if(q.try_pop(strLog))
    {
      std::cout<<strLog<<std::endl;
    }
    else
    {
      std::this_thread::yield();
    }
  }
}

template<class Lockable>
void test()
{
  threadsafe_stack<int,Lockable> s;
  threadsafe_queue<std::string> q;
  bool bDone(false);
  bool bStopProduced(false);

  std::vector<std::thread> vProduced;
  for(int i = 0; i < PRODUCER_NUMBER; ++i)
  {
    vProduced.emplace_back(std::thread(generrate_data<Lockable>, std::ref(s), std::ref(q)));
  }

  std::vector<std::thread> vConsume;
  for(int i = 0; i < CONSUMER_NUMBER; ++i)
  {
    vConsume.emplace_back(std::thread(consume_data<Lockable>, std::ref(s), std::ref(q), std::ref(bStopProduced)));
  }

  std::thread log_thread(printLog, std::ref(q), std::ref(bDone) );

  for_each(vProduced.begin(), vProduced.end(), std::mem_fn(&std::thread::join));
  bStopProduced = true;

  for_each(vConsume.begin(), vConsume.end(), std::mem_fn(&std::thread::join));
  bDone = true;

  log_thread.join();

}

int main(int argc, char *argv[])
{
  test<spin_lock>();
  return 0;
}
