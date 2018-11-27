#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <future>
#include <utility>
using namespace std;


std::vector<int> v;
atomic<bool> data_ready = {false};
int data = 0;

void read_data_p(promise<int> p, promise<bool> p2)
{
	bool ready = data_ready.load(memory_order_acquire);
	p.set_value(data);
	p2.set_value(ready);
}

void read_data()
{
	bool ready = data_ready.load(memory_order_acquire);
	cout<<"read ready: " << ready << "value: "<< data << endl;
}

void write_data()
{
	data = 1000;
	data_ready.store(true, memory_order_release);
}

void test_aquire_release()
{
	promise<int> p;
	promise<bool> p2;
	future<int> f = p.get_future();
	future<bool> f2 = p2.get_future();

	thread r(read_data_p, move(p), move(p2));
	thread r1(read_data);
	thread w(write_data);

	bool f2ready = f2.get();
	int f2value = f.get();
	cout<< "ready: " << f2ready << "value: " << f2value << endl;

	r.join();
	r1.join();
	w.join();
}


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
	test_aquire_release();
	return 0;
}
