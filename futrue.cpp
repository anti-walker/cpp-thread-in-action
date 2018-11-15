#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <future>
#include <numeric>
using namespace std;

bool isPrime(int value)
{
	for (int i = 2; i < value; ++i)
	{
		if (value %i == 0)
		{
			return false;
		}
	}
	return true;
}

void create_future_by_async()
{
	cout<< "please input a number, 0 to exit\n";
	int iValue;
	cin>>iValue;
	while(iValue)
	{
		auto f = async(isPrime, iValue);
		cout<< "is checking, please wait\n";
		cout << iValue << (f.get()? " is" : " is not ") << " prime\n" <<endl;
		cout<< "please input a number, 0 to exit\n";
		cin>>iValue;
	}
}

void my_accumulate(const std::vector<int>& v, promise<int> p)
{
	int iSum = std::accumulate(v.begin(), v.end(), 0);
	p.set_value(iSum);
}

int GetSum(const vector<int>& v)
{
	int iSum(0);
	for_each(v.begin(), v.end(), [&iSum](const int &x){
		iSum += x;
	});

	return iSum;
}

void create_future_by_promise()
{
	promise<int> p;
	std::vector<int> vnumbers = {1,2,3,4,5,6};
	future<int> f = p.get_future();
	thread t(my_accumulate, cref(vnumbers), move(p));
	f.wait();
	cout<< "result: " << f.get() <<endl;
	t.join();
}

void create_future_by_packaged_task()
{
	std::vector<int> vnumbers = {1,2,3,4,5,6};
	packaged_task<int(const vector<int>&)> task(GetSum);
	future<int> f = task.get_future();
	thread t(move(task), ref(vnumbers));

	cout<< "wait for result: \n"<<endl;
	cout<<"result: " << f.get() <<endl;
	t.join();
}

int main(int argc,char *argv[])
{
	//create_future_by_promise();
	create_future_by_packaged_task();
	return 0;
}
