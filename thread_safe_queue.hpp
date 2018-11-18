#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <utility>

template<class T>
class threadsafe_queue
{
private:
  std::queue<T> _data_queue;
  mutable std::mutex _m;
  std::condition_variable _data_cond;

public:
  threadsafe_queue()
  {

  }

  threadsafe_queue(const threadsafe_queue& r) = delete;

  void push(const T& value)
  {
    std::lock_guard<std::mutex> lock(_m);
    _data_queue.push(value);
    _data_cond.notify_one();
  }

  void wait_and_pop(T& value)
  {
    std::lock_guard<std::mutex> lock(_m);
    _data_cond.wait(lock, [this]{
      return !_data_queue.empty();
    });

    value = std::move(_data_queue.front());
    _data_queue.pop();
  }

  bool try_pop(T& value)
  {
    std::lock_guard<std::mutex> lock(_m);
    if (_data_queue.empty()) {
      return false;
    }

    value = std::move(_data_queue.front());
    _data_queue.pop();
    return true;
  }

  bool empty()
  {
    std::lock_guard<std::mutex> lock(_m);
    return _data_queue.empty();
  }
};
