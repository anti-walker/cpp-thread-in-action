#include "thread_safe_lookup_table.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <sstream>
#include <vector>


void add_to_table(threadsafe_lookup_table<std::string, int, std::hash<std::string> >& table, bool& bGo)
{
  while(!bGo)
  {
    std::this_thread::yield();
  }

  std::stringstream ss;
  ss << "xiaojiya-" << std::this_thread::get_id();
  table.add_or_update_for(ss.str(),29);

  ss.str("");
  ss << "shucdong-" << std::this_thread::get_id();
  table.add_or_update_for(ss.str(),31);

  ss.str("");
  ss << "cwenbing-" << std::this_thread::get_id();
  table.add_or_update_for(ss.str(),30);
}

void print_table(threadsafe_lookup_table<std::string, int, std::hash<std::string> >& table)
{
  std::map<std::string, int> m;
  table.get_map(m);
  for(auto& x : m)
  {
    std::cout<<x.first << ": " << x.second <<std::endl;
  }
}

void test()
{
  threadsafe_lookup_table<std::string, int, std::hash<std::string> > table;
  std::vector<std::thread> vThreads;
  auto bGo(false);
  for(int i = 0; i < 10; ++i)
  {
    vThreads.emplace_back(std::thread(add_to_table, std::ref(table), std::ref(bGo)));
  }
  bGo = true;

  for_each(vThreads.begin(), vThreads.end(), std::mem_fn(&std::thread::join));
  print_table(table);
}

int main()
{
  test();
  return 0;
}
