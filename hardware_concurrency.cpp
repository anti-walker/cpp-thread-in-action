#include <thread>
#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <mutex>
using namespace std;


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

void second_lock(mutex& m)
{
	m.lock();
	for(int i = 0; i < 10; ++i)
	{
		cout<< "  " << i <<endl;
	}

	m.unlock();
}

void first_lock(mutex& m)
{
	m.lock();
	for(int i  = 0; i < 10; ++i)
	{
		cout << i << endl;
	}
	second_lock(m); // this will deadlock

	m.unlock();
}

void test_deadlock_with_mutex()
{
	int iMaxThread = get_max_threads_count();
	mutex m;
	vector<thread> v;
	for (int i = 0; i < iMaxThread; ++i)
	{
		v.push_back(std::thread(first_lock, ref(m)));
	}

	for_each(v.begin(), v.end(), mem_fn(&thread::join));
}

template<class T>
void second_lock_guard(T& m)
{
	lock_guard<T> lock(m);
	for(int i = 0; i < 10; ++i)
	{
		cout<< "  " << i <<endl;
	}
}

template<class T>
void first_lock_guard(T& m)
{
	lock_guard<T> lock(m);
	for(int i  = 0; i < 10; ++i)
	{
		cout << i << endl;
	}
	second_lock_guard(m);
}


void test_deadlock_with_lockguard()
{
	int iMaxThread = get_max_threads_count();
	mutex m;
	vector<thread> v;
	for (int i = 0; i < iMaxThread; ++i)
	{
		v.push_back(std::thread(first_lock_guard<mutex>, ref(m)));
	}

	for_each(v.begin(), v.end(), mem_fn(&thread::join));
}

void test_recursive_mutex_with_lockguard()
{
	int iMaxThread = get_max_threads_count();
	recursive_mutex m;
	vector<thread> v;
	for (int i = 0; i < iMaxThread; ++i)
	{
		v.push_back(std::thread(first_lock_guard<recursive_mutex>, ref(m)));
	}

	for_each(v.begin(), v.end(), mem_fn(&thread::join));
}

int main(int argc,char *argv[])
{

	test_recursive_mutex_with_lockguard();
	cout<< "-----------------------" <<endl;
	test_deadlock_with_lockguard();

	return 0;
}
