#include <atomic>

class spin_lock
{
	std::atomic_flag _lock  = ATOMIC_FLAG_INIT;

public:
	spin_lock(){};

	void lock();

	void unlock();

  spin_lock(const spin_lock& r) = delete;
  spin_lock(spin_lock&& r) = delete;

  spin_lock& operator = (const spin_lock& r) = delete;
  spin_lock& operator = (spin_lock&& r) = delete;

};
