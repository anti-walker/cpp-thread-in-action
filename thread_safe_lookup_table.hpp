#include <list>
#include <functional>
#include <utility>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>

template<typename key_type, typename value_type, typename HashType=std::hash<key_type> >
class threadsafe_lookup_table
{
  class Bucket_type
  {
  private:
    // using bucket_value = std::pair<key_type, value_type>;
    // using bucket_data = std::list<bucket_value>;
    //using bucket_iterator = typename bucket_data::iterator;
    typedef std::pair<key_type, value_type> bucket_value;
    typedef std::list<bucket_value> bucket_data;
    typedef typename bucket_data::iterator bucket_iterator;

    bucket_data _data;
    mutable std::shared_mutex _m;
    friend class threadsafe_lookup_table;

    bucket_iterator find_entry_for(const key_type& k)
    {
      return std::find_if(_data.begin(), _data.end(), [&](const bucket_value& item){
        return item.first == k;
      });
    }

    const bucket_iterator find_entry_for(const key_type& k) const
    {
      return std::find_if(_data.begin(), _data.end(), [&](const bucket_value& item){
        return item.first == k;
      });
    }

  public:
    value_type value_for(const key_type& k, const value_type& default_value) const
    {
      std::shared_lock<std::shared_mutex> lock(_m);
      const bucket_iterator iter = find_entry_for(k);

      return iter==_data.end()? default_value : iter->second;
    }

    void add_or_update_for(const key_type& k, const value_type& v)
    {
      std::unique_lock<std::shared_mutex> lock(_m);
      bucket_iterator iter = find_entry_for(k);
      if (iter == _data.end())
      {
        _data.push_back(std::make_pair(k,v));
      }
      else
      {
        iter->second = v;
      }
    }

    void remove(const key_type& k)
    {
      std::unique_lock<std::shared_mutex> lock(_m);
      bucket_iterator iter = find_entry_for(k);
      if (iter!= _data.end())
      {
        _data.erase(iter);
      }
    }

  };

  std::vector<std::unique_ptr<Bucket_type> > _buckets;
  HashType _hasher;

private:
  Bucket_type& get_bucket(const key_type& key) const
  {
    const size_t index = _hasher(key)%_buckets.size();
    return *_buckets[index];
  }

public:
  threadsafe_lookup_table(unsigned num_buckets = 19, const HashType& hasher = HashType())
  :_buckets(num_buckets), _hasher(hasher)
  {
    for(auto& x : _buckets)
    {
      x = std::make_unique<Bucket_type>();
    }
  }

  threadsafe_lookup_table(const threadsafe_lookup_table& r) = delete;
  threadsafe_lookup_table& operator = (const threadsafe_lookup_table& r) = delete;

  value_type value_for(const key_type& k, const value_type& dv) const
  {
    return get_bucket(k).value_for(k,dv);
  }

  void add_or_update_for(const key_type& k, const value_type& v)
  {
    get_bucket(k).add_or_update_for(k,v);
  }

  void remove(const key_type& k)
  {
    get_bucket(k).remove(k);
  }

  void get_map(std::map<key_type,value_type>& m) const
  {
    std::vector<std::shared_lock<std::shared_mutex> > vLocks;
    for (size_t i = 0; i < _buckets.size(); ++i)
    {
      vLocks.push_back(std::shared_lock<std::shared_mutex>(_buckets[i]->_m));
    }

    for(auto& bucket : _buckets)
    {
      for(auto& item : bucket->_data)
      {
        m.insert(item);
      }
    }
  }

};
