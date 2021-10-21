#pragma once

#include <memory>
#include <string>
#include <map>

template <typename T> class trie final
{
public:

	using value_type = T;
	using key_type = std::string;

private:

	template<typename N> class node;
	template<typename N> class basic_trie_iterator;

public:

	using trie_iterator = basic_trie_iterator<value_type>;
	using const_trie_iterator = basic_trie_iterator<std::add_const_t<value_type>>;

	trie(); //default constructor
	template <class InputIterator> trie(InputIterator first, InputIterator last); //range constructor
	trie(const trie<value_type>& trie); //copy constructor
	trie(trie<value_type>&& trie); //move constructor
	~trie(); //destructor

	trie<value_type>& operator= (const trie& trie); //copy assignment
	trie<value_type>& operator= (trie&& trie); //move assignment

	[[nodiscard]] trie_iterator begin();
	[[nodiscard]] trie_iterator end();

	[[nodiscard]] const_trie_iterator cbegin() const;
	[[nodiscard]] const_trie_iterator cend() const;

	[[nodiscard]] bool empty() const;
	[[nodiscard]] size_t size() const;

	value_type& operator[] (const key_type& k);
	std::pair<trie_iterator, bool> insert(const key_type& k, const value_type& val);
	template <class InputIterator> void insert(InputIterator first, InputIterator last); //insert range of elements?

	void erase(trie_iterator position);
	size_t erase(const key_type& k);
	void erase(trie_iterator first, trie_iterator last);
	void swap(trie& trie);
	void clear();

	trie_iterator find(const key_type& k);
	const_trie_iterator find(const key_type& k) const;
	template<> class SubTrie<value_type>;

	SubTrie<> GetSubTrie(const key_type& subKey);
};


template <typename  T>
template <typename  N>
class trie<T>::basic_trie_iterator final
{
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
class trie<T>::node final
{
	value_type value_;
	std::map<char, std::shared_ptr<node>> children_;
};