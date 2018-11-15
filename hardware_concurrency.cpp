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

int main(int argc,char *argv[])
{
	int iMaxThread = std::thread::hardware_concurrency();
	iMaxThread = iMaxThread>0? iMaxThread : 2;
	cout<<"max thread number is: " << iMaxThread << endl;

	mutex m;
	vector<thread> v;
	for (int i = 0; i < iMaxThread; ++i)
	{
		v.push_back(std::thread(thread_print,i,ref(m)));
	}

	for_each(v.begin(), v.end(), mem_fn(&thread::join));

	return 0;
}