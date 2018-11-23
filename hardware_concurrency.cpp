#include <thread>
#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <mutex>
#include <atomic>
using namespace std;

class spin_lock
{
	std::atomic_flag _lock  = ATOMIC_FLAG_INIT;
public:
	spin_lock()
	{

	}

	void lock()
	{
		while(_lock.test_and_set())
		{
			std::this_thread::yield();
		}
	}

	void unlock()
	{
		_lock.clear();
	}

};

void thread_print(int i,mutex& m)
{
	lock_guard<mutex> lock(m);
	cout<< "hello world! i am the number: " << i << " and my id is: "<< std::this_thread::get_id() << endl;
}

int get_max_threads_count()
{
	int iMaxThread = std::thread::hardware_concurrency();
	iMaxThread = iMaxThread>0? iMaxThread : 2;
	cout<<"max thread number is: " << iMaxThread << endl;

	return iMaxThread;
}

template<class Lockable>
void second_lock_guard(Lockable& m)
{
	lock_guard<Lockable> lock(m);
	for(int i = 0; i < 10; ++i)
	{
		cout<< "  " << i <<endl;
	}
}

template<class Lockable>
void first_lock_guard(Lockable& m)
{
	lock_guard<Lockable> lock(m);
	for(int i  = 0; i < 10; ++i)
	{
		cout << i << endl;
	}
	second_lock_guard(m);
}

template<class MutexType>
void test_deadlock_by_recursive_with_lockguard(MutexType& m)
{
	int iMaxThread = get_max_threads_count();
	vector<thread> v;
	for (int i = 0; i < iMaxThread; ++i)
	{
		v.push_back(std::thread(first_lock_guard<MutexType>, ref(m)));
	}

	for_each(v.begin(), v.end(), mem_fn(&thread::join));
}

int main(int argc,char *argv[])
{
	// recursive_mutex rm;
	// test_deadlock_by_recursive_with_lockguard(rm);// this won't deadlock
	//
	// cout<< "-------------------------------------------------------------" <<endl;
	//
	// mutex m;
	// test_deadlock_by_recursive_with_lockguard(m); // this will deadlock

	cout<< "-------------------------------------------------------------" <<endl;
	spin_lock l;
	test_deadlock_by_recursive_with_lockguard(l);


	return 0;
}
