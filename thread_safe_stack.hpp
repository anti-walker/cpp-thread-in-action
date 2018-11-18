#include <exception>
#include <stack>
#include <mutex>
#include <memory>


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
  mutable std::mutex _m;

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
