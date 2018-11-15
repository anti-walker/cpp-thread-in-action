#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
using namespace std;


std::vector<int> v;
atomic<bool> data_ready = {false};

void read_thread()
{
	while(!data_ready.load())
	{
		cout<< "waiting..." <<endl;
		std::this_thread::sleep_for(1ms);
	}

	cout<< "the result: is " << v[0] <<endl;
}

void write_thread()
{
	v.push_back(42);
	data_ready = true;
}

atomic<bool> x,y;
atomic<int> z;
bool go = false;

void write_x_then_y()
{
	while(!go)
		this_thread::yield();

	x.store(true,memory_order_relaxed);
	y.store(true,memory_order_relaxed);
}

void read_y_then_x()
{
	while(!go)
		this_thread::yield();

	cout<<"y: " << y.load(memory_order_relaxed) <<endl;
	cout<<"x: " << x.load(memory_order_relaxed) <<endl;;
}

void test1()
{
	x = false;
	y = false;

	thread a(read_y_then_x);
	thread b(write_x_then_y);
	go = true;
	a.join();
	b.join();
}


int main(int argc,char *argv[])
{
	return 0;
}
