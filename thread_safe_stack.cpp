#include <exception>
#include <stack>
#include <mutex>
#include <memory>
#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include <algorithm>

const int LOOP_NUMBER = 1000;
const int PRODUCER_NUMBER = 2;
const int CONSUMER_NUMBER = 3;

class empty_stack : public std::exception
{
  const char* what() const throw();
};

const char* empty_stack::what() const throw()
{
  return "empty in stack";
}

template<class T>
class threadsafe_stack
{
private:
  std::stack<T> _data;
  std::mutex _m;

public:
  threadsafe_stack() {}
  threadsafe_stack(const threadsafe_stack& r)
  {
    std::lock_guard<std::mutex> lock(_m);
    _data = r._data;
  }

  threadsafe_stack& operator = (const threadsafe_stack& ) = delete;

  void push(const T& value)
  {
    std::lock_guard<std::mutex> lock(_m);
    _data.push(value);
  }

  std::shared_ptr<T> pop()
  {
    std::lock_guard<std::mutex> lock(_m);
    if (_data.empty())
    {
      throw empty_stack();
    }
    std::shared_ptr<T> value(std::make_shared<T>(_data.top()));
    _data.pop();

    return value;
  }

  void pop(T& value)
  {
    std::lock_guard<std::mutex> lock(_m);
    if (_data.empty())
    {
      throw empty_stack();
    }
    value = _data.top();
    _data.pop();
  }

  bool empty() const
  {
    std::lock_guard<std::mutex> lock(_m);
    return _data.empty();
  }
};

void generrate_data(threadsafe_stack<int>& s)
{
  for(int i = 0; i < LOOP_NUMBER; ++i)
  {
    s.push(i);
    std::cout<< "produced: " << i <<std::endl;
    std::this_thread::yield();
  }
}

void consume_data(threadsafe_stack<int>& s)
{
  for(int i = 0; i < LOOP_NUMBER; ++i)
  {
    int value;
    try
    {
      s.pop(value);
      std::cout<< "consumed: " << value <<std::endl;
    }
    catch(std::exception& e)
    {
      std::cout<< e.what() <<std::endl;
    }

    std::this_thread::yield();
  }
}

void test()
{
  std::vector<std::thread> v;
  threadsafe_stack<int> s;

  for(int i = 0; i < PRODUCER_NUMBER; ++i)
  {
    v.emplace_back(std::thread(generrate_data, std::ref(s)));
  }

  for(int i = 0; i < CONSUMER_NUMBER; ++i)
  {
    v.emplace_back(std::thread(consume_data, std::ref(s)));
  }

  for_each(v.begin(), v.end(), std::mem_fn(&std::thread::join));

}

int main(int argc, char *argv[])
{
  test();
  return 0;
}
