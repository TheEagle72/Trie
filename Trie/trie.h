#pragma once

#include <string>
#include <map>
#include <memory>
#include <stdexcept>

template <typename T>
class trie final
{
public:

	using value_type = T;
	using key_type = std::string;

private:

	class node;
	template<typename N> class basic_trie_iterator;

	node root_;

public:

	using iterator = basic_trie_iterator<value_type>;
	using const_iterator = basic_trie_iterator<std::add_const_t<value_type>>;

	trie(); //default constructor
	template <class InputIterator> trie(InputIterator first, InputIterator last); //range constructor
	trie(const trie<value_type>& trie); //copy constructor
	trie(trie<value_type>&& trie) noexcept; //move constructor
	~trie() = default; //destructor

	trie<value_type>& operator= (const trie& trie); //copy assignment
	trie<value_type>& operator= (trie&& trie); //move assignment

	[[nodiscard]] iterator begin();
	[[nodiscard]] iterator end();

	[[nodiscard]] const_iterator cbegin() const;
	[[nodiscard]] const_iterator cend() const;

	[[nodiscard]] bool empty() const;
	[[nodiscard]] size_t size() const;

	value_type& operator[] (const key_type& key);
	std::pair<iterator, bool> insert(const key_type& key, const value_type& value);
	template <class InputIterator> void insert(InputIterator first, InputIterator last); //insert range of elements?

	void erase(iterator position);
	size_t erase(const key_type& k);
	void erase(iterator first, iterator last);
	void swap(trie& trie);
	void clear();

	iterator find(const key_type& k);
	const_iterator find(const key_type& k) const;

	class SubTrie;
	SubTrie GetSubTrie(const key_type& subKey);
};

template <typename T>
class trie<T>::node final
{
	class node_exception;
	key_type key_;
	value_type value_;
	std::map<key_type, std::shared_ptr<node>> children_;
public:
	size_t size() const;
	bool empty() const;
	node(key_type key, value_type value);
	void insert();
	bool is_exist(const key_type&);
	std::shared_ptr<node> at(const key_type& key);
};

template <typename T>
class trie<T>::node::node_exception final: public std::runtime_error
{
	node_exception(const std::string& msg) : runtime_error(msg) {}
};

template <typename T>
trie<T>::trie() : root_({ {}, {} }) {}

template <typename T>
std::pair<typename trie<T>::iterator, bool> trie<T>::insert(const key_type& key, const value_type& value)
{
	bool is_existed = true;
	for (const auto letter : key)
	{
		const std::string k{ letter };
		std::shared_ptr<node> current_node;

		if (root_.is_exist(k))
		{
			current_node = root_.at(k);
		}
		else
		{
			current_node = std::make_shared<node>(k, value_type{});
		}
	}

	basic_trie_iterator<const value_type&> it(value);
	return std::make_pair(it, is_existed);
}

template <typename T>
size_t trie<T>::node::size() const
{
	return children_.size();
}

template <typename T>
bool trie<T>::node::empty() const
{
	return children_.empty();
}

template <typename T>
trie<T>::node::node(key_type key, value_type value) : key_(std::move(key)), value_(std::move(value)) {}

template <typename T>
std::shared_ptr<typename trie<T>::node> trie<T>::node::at(const key_type& key)
{
	if (children_.count(key))
	{
		return children_.at(key);
	}
	throw node_exception("out_of_range");
}

template <typename  T>
template <typename  N>
class trie<T>::basic_trie_iterator final
{
	std::shared_ptr<node> ptr_;
public:

	using iterator_category = std::forward_iterator_tag;
	using value_type = N;
	using pointer = N*;
	using reference = N&;

	basic_trie_iterator(value_type& x);
	basic_trie_iterator(const basic_trie_iterator& mit);

	basic_trie_iterator& operator++();
	basic_trie_iterator operator++(int);

	bool operator==(const basic_trie_iterator& rhs);
	bool operator!=(const basic_trie_iterator& rhs);

	value_type operator*();
	value_type* operator->();
};

template <typename T>
template <typename N>
trie<T>::basic_trie_iterator<N>::basic_trie_iterator(value_type& x)
{

}
