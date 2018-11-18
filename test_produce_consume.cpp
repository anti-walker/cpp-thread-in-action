#include "thread_safe_stack.hpp"
#include "thread_safe_queue.hpp"
#include <vector>
#include <thread>
#include <iostream>
#include <sstream>

const int LOOP_NUMBER = 1000;
const int PRODUCER_NUMBER = 2;
const int CONSUMER_NUMBER = 3;

void generrate_data(threadsafe_stack<int>& s, threadsafe_queue<std::string>& q)
{
  std::string strLog;
  for(int i = 0; i < LOOP_NUMBER; ++i)
  {
    s.push(i);

    std::stringstream ss;
    ss << "produced: " << i;
    strLog = ss.str();
    q.push(strLog);
  }
}

void consume_data(threadsafe_stack<int>& s, threadsafe_queue<std::string>& q)
{
  std::string strLog;
  for(int i = 0; i < LOOP_NUMBER; ++i)
  {
    int value;
    try
    {
      s.pop(value);

      std::stringstream ss;
      ss << "consumed: " << value;
      strLog = ss.str();
      q.push(strLog);
    }
    catch(std::exception& e)
    {
      std::stringstream ss;
      ss << e.what() << "while consume";
      strLog = ss.str();
      q.push(strLog);
    }

    std::this_thread::yield();
  }
}

void printLog(threadsafe_queue<std::string>& q, bool& bDone)
{
  std::string strLog;
  while(!bDone)
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

void test()
{
  std::vector<std::thread> v;
  threadsafe_stack<int> s;
  threadsafe_queue<std::string> q;
  bool bDone(false);

  for(int i = 0; i < PRODUCER_NUMBER; ++i)
  {
    v.emplace_back(std::thread(generrate_data, std::ref(s), std::ref(q)));
  }

  for(int i = 0; i < CONSUMER_NUMBER; ++i)
  {
    v.emplace_back(std::thread(consume_data, std::ref(s), std::ref(q)));
  }

  std::thread log_thread(printLog, std::ref(q), std::ref(bDone));

  for_each(v.begin(), v.end(), std::mem_fn(&std::thread::join));

  bDone = true;

  log_thread.join();

}

int main(int argc, char *argv[])
{
  test();
  return 0;
}
