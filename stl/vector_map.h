#ifndef __VECTOR_MAP_H__ 
#define __VECTOR_MAP_H__ 
#include <vector> 
#include <algorithm> 
#include <utility> 
#include <functional>
//wrapped vector for map 
template<typename key_t, typename mapped_t, class compare_t = std::less<key_t> > 
class vector_map_basic: public std::vector<std::pair<key_t, mapped_t> > { 
public: 
    typedef compare_t key_compare; 
    typedef key_t key_type; 
    typedef mapped_t mapped_type; 
    typedef std::pair<key_t, mapped_t> value_type; 
    typedef vector_map_basic<key_t, mapped_t, compare_t> this_type; 
    typedef std::vector<value_type> container_type;
#define THIS_TYPE vector_map_basic<key_t, mapped_t, compare_t>
    void push_back(key_type const &key, mapped_type const &mapped) { 
        push_back(make_pair(key, mapped)); 
    }
    class compare_for_find: public std::binary_function<value_type, key_type, 
            bool> { 
    public: 
        explicit compare_for_find(key_compare const &comp) : 
            comp_(comp) { 
        } 
        inline 
        bool operator()(value_type const &node, key_type const &key) { 
            return comp_(node.first, key); 
        }
        key_compare comp_; 
    };
    class compare_for_sort: public std::binary_function<value_type, value_type, 
            bool> { 
    public: 
        explicit compare_for_sort(key_compare const &comp) : 
            comp_(comp) { 
        }
        inline 
        bool operator()(value_type const &left, value_type const &right) { 
            return comp_(left.first, right.first); 
        } 
        key_compare comp_; 
    };
    //get 
    bool get(key_type const &key, mapped_type & result) const { 
        typename this_type::const_iterator it = find(key); 
        if (it != data().end()) { 
            result = it->second; 
            return true; 
        } 
        return false; 
    }
    inline std::pair<typename THIS_TYPE::const_iterator, 
            typename THIS_TYPE::const_iterator> equal_range(key_type const &key) const { 
        return std::equal_range(data().begin(), data().end(), key, 
                compare_for_find(key_comp_)); 
    }
    inline std::pair<typename THIS_TYPE::iterator, typename THIS_TYPE::iterator> equal_range( 
            key_type const &key) { 
        return std::equal_range(data().begin(), data().end(), key, 
                compare_for_find(key_comp_)); 
    }
    typename THIS_TYPE::const_iterator find(key_type const &key) const { 
        typename this_type::const_iterator it = std::lower_bound( 
                data().begin(), data().end(), key, compare_for_find(key_comp_)); 
        typename this_type::iterator iend = data().end(); 
        if (it != iend && !key_comp_(key, it->first)) 
            return it; 
        return iend; 
    }
    typename THIS_TYPE::iterator find(key_type const &key) { 
        typename this_type::iterator it = std::lower_bound(data().begin(), 
                data().end(), key, compare_for_find(key_comp_)); 
        typename this_type::iterator iend = data().end(); 
        if (it != iend && !key_comp_(key, it->first)) 
            return it; 
        return iend; 
    }
    key_compare key_comp() const { 
        return key_comp_; 
    }
    //sort 
    void sort() { 
        typename this_type::compare_for_sort comp(this->key_comp_); 
        std::sort(data().begin(), data().end(), comp); 
    }
    // 
    void set_default_value(mapped_type const &def) { 
        mapped_default_ = def; 
    }
    mapped_type default_value() const { 
        return mapped_default_; 
    }
    typename THIS_TYPE::container_type const & 
    data() const { 
        return *this; 
    }
    typename THIS_TYPE::container_type & 
    data() { 
        return *this; 
    }
public: 
    key_compare key_comp_; 
    mapped_type mapped_default_;
#undef THIS_TYPE 
};
//wrapped vector for map 
template<typename key_t, typename mapped_t, class compare_t = std::less<key_t> > 
class vector_map: public vector_map_basic<key_t, mapped_t, compare_t> { 
public:
#define THIS_TYPE vector_map<key_t, mapped_t,compare_t>
    typedef vector_map this_type; 
    typedef key_t key_type; 
    typedef mapped_t mapped_type; 
    typedef vector_map_basic<key_t, mapped_t, compare_t> parent_type;
    typedef std::pair<key_t, mapped_t> value_type; 
    vector_map() { 
    }
    //for fast insert
    void push_back(typename THIS_TYPE::value_type const &value) { 
        push_back(value); 
    }
    void push_back(typename THIS_TYPE::key_type const &key, 
            typename THIS_TYPE::mapped_type const &mapped) { 
        push_back(make_pair(key, mapped)); 
    }
    void set(typename THIS_TYPE::key_type const &key, 
            typename THIS_TYPE::mapped_type const & val) { 
        typename this_type::iterator iend = this->data().end(), it = 
                std::lower_bound(this->data().begin(), iend, key, 
                        typename this_type::compare_for_find(this->key_comp_)); 
        if (it != iend && !key_comp_(key, it->first)) { 
            it -> second = val; 
        } else { 
            insert(it, make_pair(key, val)); 
        } 
    }
    //like map insert 
    void insert(typename this_type::value_type const &value) { 
        this->set(value.first, value.second); 
    }
    void insert(typename this_type::iterator pos, 
            typename this_type::value_type const &value) { 
        this->data().insert(pos, value); 
    }
    //get 
    bool get(typename THIS_TYPE::key_type const &key, 
            typename THIS_TYPE::mapped_type & result) const { 
        return parent_type::get(key, result); 
    }
    typename THIS_TYPE::mapped_type const & 
    operator[](typename THIS_TYPE::key_type const &key) const { 
        typename this_type::mapped_type result = this->mapped_default_; 
        get(key, result); 
        return result; 
    }
    typename THIS_TYPE::mapped_type & 
    operator[](typename THIS_TYPE::key_type const &key) { 
        typename this_type::iterator iend = this->data().end(); 
        typename this_type::iterator it = std::lower_bound( 
                this->data().begin(), iend, key, 
                typename this_type::compare_for_find(this->key_comp_)); 
        if (it != iend && !key_comp_(key, it->first)) { 
            return it -> second; 
        } 
        return this->data().insert(it, make_pair(key, this->mapped_default_))->second; 
    } 
#undef THIS_TYPE 
};
template<typename like_map_t, typename val_t> 
void clear_value(like_map_t &map, val_t const & null_val) { 
    for (typename like_map_t::iterator it = map.begin(), iend = map.end(); it 
            != iend; ++it) { 
        it->second = null_val; 
    } 
}
template<typename like_map_t> 
void clear_value(like_map_t &map) { 
    clear_value(map, typename like_map_t::mapped_type()); 
}
#endif //__VECTOR_MAP_H__
