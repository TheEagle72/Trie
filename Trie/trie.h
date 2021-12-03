#pragma once

#include <algorithm>
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

	std::shared_ptr<node> root_;
	size_t size_ = 0;

public:

	using iterator = basic_trie_iterator<value_type>;
	using const_iterator = basic_trie_iterator<std::add_const_t<value_type>>;

	trie();//default constructor
	template <typename InputIterator> trie(InputIterator first, InputIterator last); //range constructor
	trie(const trie<value_type>& other); //copy constructor
	trie(trie<value_type>&& other) noexcept; //move constructor
	~trie() = default; //destructor

	trie<value_type>& operator= (const trie& other); //copy assignment
	trie<value_type>& operator= (trie&& other) noexcept; //move assignment
	bool operator==(const trie& other);

	[[nodiscard]] iterator begin();
	[[nodiscard]] iterator end();

	[[nodiscard]] const_iterator cbegin() const;
	[[nodiscard]] const_iterator cend() const;

	[[nodiscard]] bool empty() const;
	[[nodiscard]] size_t size() const;

	value_type& operator[] (const key_type& key);
	std::pair<iterator, bool> insert(const key_type& key, const value_type& value);
	std::pair<iterator, bool> insert_or_assign(const key_type& key, const value_type& value);
	template <typename InputIterator> void insert(InputIterator first, InputIterator last); //insert range of elements?
	template <typename InputIterator> void insert_or_assign(InputIterator first, InputIterator last); //insert range of elements?

	void erase(iterator position);
	size_t erase(const key_type& k);
	void erase(iterator first, iterator last);
	void swap(trie& other) noexcept;
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
	bool have_value_ = false;

	std::map<key_type, std::shared_ptr<node>> children_;
public:
	size_t size() const;
	bool empty() const;
	node() = default;
	node(const key_type& key);
	node(const key_type& key, const value_type& value);
	void insert(const key_type& key, std::shared_ptr<node> node_);
	void insert_or_assign(const key_type& key, std::shared_ptr<node> node_);
	bool have_child(const key_type&) const;
	bool have_value() const;
	void set_value(value_type&& value);
	value_type value() const;
	std::shared_ptr<node> get_parent() const;
	std::shared_ptr<node> at(const key_type& key);
};

template <typename T>
class trie<T>::node::node_exception final: public std::runtime_error
{
public:
	node_exception(const std::string & msg) : runtime_error(msg) {}
};

template <typename T>
trie<T>::trie() :root_(std::make_shared<node>()) {}

template <typename T>
template <typename InputIterator>
trie<T>::trie(InputIterator first, InputIterator last)
{
	insert(first, last);
}

template <typename T>
trie<T>::trie(const trie<value_type>& other) { *this = other; }

template <typename T>
trie<T>::trie(trie<value_type>&& other) noexcept { *this = std::move(other); }

template <typename T>
trie<typename trie<T>::value_type>& trie<T>::operator=(const trie& other)
{
	/*if (this == other) todo fix issue with comparision
	{
		return *this;
	}*/
	
	root_.reset(); //todo for some reason root_.reset(other.root_); not working 
	root_ = other.root_;

	size_ = other.size();
	return *this;
}

template <typename T>
trie<typename trie<T>::value_type>& trie<T>::operator=(trie&& other) noexcept
{
	root_.reset();
	root_ = std::move(other.root_);

	size_ = other.size();
	other.size_ = 0;
	return *this;
}

template <typename T>
bool trie<T>::operator==(const trie& other)
{
	if (size_!=other.size())
	{
		return false;
	}

	auto current_node_this = root_;
	auto current_node_other = other.root_;
	return false;
}

template <typename T>
typename trie<T>::iterator trie<T>::begin()
{
	return iterator(root_->children_.begin()->second);
}

template <typename T>
bool trie<T>::empty() const
{
	return root_;
}

template <typename T>
size_t trie<T>::size() const
{
	return size_;
}

template <typename T>
std::pair<typename trie<T>::iterator, bool> trie<T>::insert(const key_type& key, const value_type& value)
{
	std::shared_ptr<node> current_node{ root_ };
	bool was_inserted = false;

	for (size_t i = 0; i < key.length() - 1; ++i)
	{
		std::string current_letter{ key.at(i) };
		if (!current_node->have_child(current_letter))
		{
			current_node->insert(current_letter, std::make_shared<node>(current_letter));
			++size_;
		}
		current_node = current_node->at(current_letter);
	}

	std::string last_letter{ key.back() };
	if (!current_node->have_child(last_letter))
	{
		current_node->insert(last_letter, std::make_shared<node>(last_letter, value));
		current_node = current_node->at(last_letter);
		++size_;
		was_inserted = true;
	}

	return std::make_pair(iterator{current_node}, was_inserted);
}

template <typename T>
std::pair<typename trie<T>::iterator, bool> trie<T>::insert_or_assign(const key_type& key, const value_type& value)
{
	std::shared_ptr<node> current_node{ root_ };
	bool was_inserted = false;

	for (size_t i = 0; i < key.length() - 1; ++i)
	{
		std::string current_letter{ key.at(i) };
		if (!current_node->have_child(current_letter))
		{
			current_node->insert(current_letter, std::make_shared<node>(current_letter));
			++size_;
		}
		current_node = current_node->at(current_letter);
	}

	std::string last_letter{ key.back() };
	if (!current_node->have_child(last_letter))
	{
		current_node->insert(last_letter, std::make_shared<node>(last_letter, std::forward<value_type>(value)));
		current_node = current_node->at(last_letter);
		++size_;
		was_inserted = true;
	}
	else
	{
		//assign
		current_node = current_node->at(last_letter);
		current_node->set_value(value);
	}

	return std::make_pair(iterator{ current_node }, was_inserted);
}

template <typename T>
template <class InputIterator>
void trie<T>::insert(InputIterator first, InputIterator last)
{
	if (first == last)
	{
		return;
	}

	auto it = first;
	do
	{
		insert(it.first, it.second);
		++it;
	} while (first != last);
}

template <typename T>
void trie<T>::swap(trie& other) noexcept
{
	root_.swap(other.root_);
	std::swap(size_, other.size_);
}

template <typename T>
void trie<T>::clear()
{
	root_.reset();
	size_ = 0;
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
trie<T>::node::node(const key_type& key) : key_(key), value_() {}

template <typename T>
trie<T>::node::node(const key_type& key, const value_type& value) : key_(key), value_(value) {}

template <typename T>
void trie<T>::node::insert(const key_type& key, std::shared_ptr<node> node_)
{
	have_value_ = true;
	children_.insert({ key, std::forward<std::shared_ptr<node>>(node_) });
}

template <typename T>
void trie<T>::node::insert_or_assign(const key_type& key, std::shared_ptr<node> node_)
{
	have_value_ = true;
	children_.insert_or_assign({ key, std::forward<std::shared_ptr<node>>(node_) });
}

template <typename T>
bool trie<T>::node::have_child(const key_type& key) const
{
	return children_.count(key);
}

template <typename T>
bool trie<T>::node::have_value() const
{
	return have_value_;
}

template <typename T>
void trie<T>::node::set_value(value_type&& value)
{
	have_value_ = true;
	value_ = value;
}

template <typename T>
typename trie<T>::value_type trie<T>::node::value() const
{
	return value_;
}

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
	std::weak_ptr<node> ptr_;
public:

	using iterator_category = std::forward_iterator_tag;
	using value_type = N;
	using pointer = N*;
	using reference = N&;

	basic_trie_iterator(const std::shared_ptr<node>& node_);
	basic_trie_iterator(value_type& x);
	basic_trie_iterator(const basic_trie_iterator& other);

	basic_trie_iterator& operator++();
	basic_trie_iterator operator++(int);

	bool operator==(const basic_trie_iterator& rhs);
	bool operator!=(const basic_trie_iterator& rhs);

	value_type operator*();
	value_type* operator->();
};

template <typename T>
template <typename N>
trie<T>::basic_trie_iterator<N>::basic_trie_iterator(const std::shared_ptr<node>& node_) : ptr_(node_) {}

template <typename T>
template <typename N>
trie<T>::basic_trie_iterator<N>::basic_trie_iterator(value_type& x) {}

template <typename T>
template <typename N>
trie<T>::basic_trie_iterator<N>::basic_trie_iterator(const basic_trie_iterator& other) : ptr_(other.ptr_) {}

template <typename T>
template <typename N>
trie<T>::basic_trie_iterator<N>& trie<T>::basic_trie_iterator<N>::operator++()
{
	if (ptr_.expired())
	{
		return end();
	}

	std::shared_ptr<node> shared = ptr_.lock();

	return {};
}

template <typename T>
template <typename N>
bool trie<T>::basic_trie_iterator<N>::operator==(const basic_trie_iterator& rhs)
{
	return (ptr_ == rhs.ptr_);
}

