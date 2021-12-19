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
	using pointer = std::add_pointer<value_type>;
	using reference = std::add_lvalue_reference<value_type>;
private:

	class node;
	template<bool IsConst> class basic_trie_iterator;

	size_t size_ = 0;
	std::shared_ptr<node> root_;
	std::shared_ptr<node> end_;
	trie(std::shared_ptr<node> root, std::shared_ptr<node> end);
public:

	using iterator = basic_trie_iterator<false>;
	using const_iterator = basic_trie_iterator<true>;

	trie();//default constructor
	template <typename InputIterator> trie(InputIterator first, InputIterator last); //range constructor
	trie(const trie& other); //copy constructor
	trie(trie&& other) noexcept; //move constructor
	~trie() = default; //destructor

	trie& operator= (const trie& other); //copy assignment
	trie& operator= (trie&& other) noexcept; //move assignment
	bool operator==(const trie& other) noexcept;

	[[nodiscard]] iterator begin() noexcept;
	[[nodiscard]] const_iterator begin() const noexcept;

	[[nodiscard]] iterator end() noexcept;
	[[nodiscard]] const_iterator end() const noexcept;

	[[nodiscard]] const_iterator cbegin() const noexcept;
	[[nodiscard]] const_iterator cend() const noexcept;

	[[nodiscard]] bool empty() const;
	[[nodiscard]] size_t size() const;

	value_type& operator[] (const key_type& key);

	std::pair<iterator, bool> insert(const key_type& key, const value_type& value);
	std::pair<iterator, bool> insert_or_assign(const key_type& key, const value_type& value);
	template <typename InputIterator> void insert(InputIterator first, InputIterator last); //add_child range of elements?
	template <typename InputIterator> void insert_or_assign(InputIterator first, InputIterator last); //add_child range of elements?

	void erase(iterator position);
	size_t erase(const key_type& key);
	void erase(iterator first, iterator last);

	void swap(trie& other) noexcept;
	void clear();

	[[nodiscard]] iterator find(const key_type& key);
	[[nodiscard]] const_iterator find(const key_type& key) const;

	trie get_sub_trie(const key_type& key);
};

template <typename T>
class trie<T>::node final
{
	class node_exception;
	std::weak_ptr<node> parent_;
	std::pair<const char, value_type> key_and_value_;
	std::map<char, std::shared_ptr<node>> children_;
	bool is_key_ = false;
	size_t size_ = 0;
public:

	node() = default;
	node(std::weak_ptr<node> parent, const std::pair<char, value_type>& key_and_value);
	void add_child(const std::pair<char, std::shared_ptr<node>>& key_and_node, bool is_key = false);
	size_t remove_child(char key); // return how many nodes was deleted
	[[nodiscard]] std::shared_ptr<node> get_child(char key);
	[[nodiscard]] bool is_child(char key) const;

	void set_value(const value_type& value);
	[[nodiscard]] bool is_key() const;
	[[nodiscard]] bool is_value() const;

	[[nodiscard]] std::pair<const char, value_type>& operator*();
	[[nodiscard]] std::pair<const char, value_type>* operator->();

	[[nodiscard]] bool have_parent() const;
	[[nodiscard]] std::weak_ptr<node> get_parent() const;

	[[nodiscard]] auto find(char key);
	[[nodiscard]] bool empty() const noexcept;
	[[nodiscard]] typename std::map<char, std::shared_ptr<node>>::iterator begin() noexcept;
	[[nodiscard]] typename std::map<char, std::shared_ptr<node>>::iterator end() noexcept;
	[[nodiscard]] typename std::map<char, std::shared_ptr<node>>::const_iterator begin() const noexcept;
	[[nodiscard]] typename std::map<char, std::shared_ptr<node>>::const_iterator end() const noexcept;
};

template <typename  T>
template <bool  IsConst>
class trie<T>::basic_trie_iterator final
{
	friend basic_trie_iterator<false>;
	friend basic_trie_iterator<true>;
	trie& my_trie_;
	std::weak_ptr<node> ptr_;

	[[nodiscard]] std::weak_ptr<node> get_node();
	[[nodiscard]] std::weak_ptr<node> get_node() const;
public:
	using iterator_category = std::forward_iterator_tag;
	using value_type = value_type;
	using pointer = pointer;
	using reference = reference;

	basic_trie_iterator(trie& my_trie, std::shared_ptr<node> node);
	basic_trie_iterator(trie& my_trie, const basic_trie_iterator& other);

	basic_trie_iterator& operator++();
	basic_trie_iterator operator++(int);

	bool operator==(const basic_trie_iterator& rhs) const;
	bool operator!=(const basic_trie_iterator& rhs) const;

	basic_trie_iterator& operator=(const basic_trie_iterator& other);

	std::pair<const char, value_type>& operator*();
	std::pair<const char, value_type>* operator->();

	key_type get_full_key() const;
	[[nodiscard]] bool is_key() const;
	size_t reset();
};


template <typename T>
class trie<T>::node::node_exception final: public std::runtime_error
{
public:
	node_exception(const std::string & msg) : runtime_error(msg) {}
};

template <typename T>
trie<T>::trie(std::shared_ptr<node> root, std::shared_ptr<node> end) : root_(root), end_(end) {}

template <typename T>
trie<T>::trie() : root_(std::make_shared<node>()), end_(std::make_shared<node>()) {}

template <typename T>
template <typename InputIterator>
trie<T>::trie(InputIterator first, InputIterator last)
{
	auto it = first;
	while (it != last)
	{
		insert(*it.first, *it.second);
	}
}

template <typename T>
trie<T>::trie(const trie<value_type>& other) { *this = other; }

template <typename T>
trie<T>::trie(trie<value_type>&& other) noexcept { *this = std::move(other); }

template <typename T>
trie<typename trie<T>::value_type>& trie<T>::operator=(const trie& other)
{
	if (this == &other)
	{
		return *this;
	}

	root_.reset();
	root_ = std::make_shared<node>();
	auto it = other.begin();

	while (it != other.end())
	{
		insert_or_assign(it.get_full_key(), it->second);
		++it;
	}

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
bool trie<T>::operator==(const trie& other) noexcept
{
	if (size() != other.size())
	{
		return false;
	}

	auto it_this = begin();
	auto it_other = other.begin();
	for (size_t i = 0; i < size(); ++i)
	{
		if (*it_this != *it_other)
		{
			return false;
		}
	}
	return true;
}

template <typename T>
typename trie<T>::iterator trie<T>::begin() noexcept
{
	return iterator{ *this, root_->begin()->second };
}

template <typename T>
typename trie<T>::const_iterator trie<T>::begin() const noexcept
{
	return const_iterator{ const_cast<trie&>(*this), root_->begin()->second };
}

template <typename T>
typename trie<T>::iterator trie<T>::end() noexcept
{
	return iterator{ *this, end_ };
}

template <typename T>
typename trie<T>::const_iterator trie<T>::end() const noexcept
{
	return const_iterator{ const_cast<trie&>(*this), end_ };
}


template <typename T>
typename trie<T>::const_iterator trie<T>::cbegin() const noexcept
{
	return begin();
}

template <typename T>
typename trie<T>::const_iterator trie<T>::cend() const noexcept
{
	return end();
}

template <typename T>
bool trie<T>::empty() const
{
	return root_->empty();
}

template <typename T>
size_t trie<T>::size() const
{
	return size_;
}

template <typename T>
typename trie<T>::value_type& trie<T>::operator[](const key_type& key)
{
	auto it = find(key);
	if (it == end())
	{
		it = insert(key, {}).first;
	}

	return *it;
}

template <typename T>
std::pair<typename trie<T>::iterator, bool> trie<T>::insert(const key_type& key, const value_type& value)
{
	std::shared_ptr<node> current_node{ root_ };
	bool was_inserted;

	for (size_t i = 0; i < key.length() - 1; ++i)
	{
		const char current_letter = key.at(i);
		if (!current_node->is_child(current_letter))
		{
			current_node->add_child(std::make_pair(current_letter, std::make_shared<node>(current_node, std::make_pair(current_letter, value))));
		}
		current_node = current_node->get_child(current_letter);
	}

	const char last_letter = key.back();
	if (!current_node->is_child(last_letter))
	{
		current_node->add_child(std::make_pair(last_letter, std::make_shared<node>(current_node, std::make_pair(last_letter, value))));
		current_node = current_node->get_child(last_letter);
		++size_;
		was_inserted = true;
	}

	return { iterator(*this, current_node), was_inserted };
};

template <typename T>
std::pair<typename trie<T>::iterator, bool> trie<T>::insert_or_assign(const key_type& key, const value_type& value)
{
	auto [it, was_inserted] = insert(key, value);
	if (!was_inserted)
	{
		it->second = value;
	}
	return { it, was_inserted };
}

template <typename T>
template <class InputIterator>
void trie<T>::insert(InputIterator first, InputIterator last)
{
	auto it = first;
	while (it != last)
	{
		insert(it.first, it.second);
		++it;
	}
}

template <typename T>
template <typename InputIterator>
void trie<T>::insert_or_assign(InputIterator first, InputIterator last)
{
	auto it = first;
	while (it != last)
	{
		insert_or_assign(it.first, it.second);
		++it;
	}
}

template <typename T>
void trie<T>::erase(iterator position)
{
	if (position != end())
	{
		size_ -= position.reset();
	}
}

template <typename T>
size_t trie<T>::erase(const key_type& key)
{
	auto it = find(key);
	if (it != end())
	{
		erase(it);
		return 1;
	}
	return 0;
}

template <typename T>
void trie<T>::erase(iterator first, iterator last)
{
	auto it = first;

	if (it == end())
	{
		return;
	}

	while (it != end() && it != last)
	{
		erase(it);
	}
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
typename trie<T>::iterator trie<T>::find(const key_type& key)
{
	auto current_node = root_;

	for (const char current_letter : key)
	{
		if (!current_node->is_child(current_letter))
		{
			return end();
		}
		current_node = current_node->get_child(current_letter);
	}

	return iterator{ *this, current_node };
}

template <typename T>
typename trie<T>::const_iterator trie<T>::find(const key_type& key) const
{
	return const_iterator(find(key));
}

template <typename T>
trie<T> trie<T>::get_sub_trie(const key_type& key)
{
	auto it = find(key);
	if (it == end())
	{
		throw std::exception("invalid key");
	}

	auto node = it.get_node().lock();
	auto parent = node->get_parent().lock();

	auto start_node = parent->get_child(node->first);
	return trie(start_node, parent);
}


template <typename T>
trie<T>::node::node(std::weak_ptr<node> parent, const std::pair<char, value_type>& key_and_value) : parent_(parent), key_and_value_(key_and_value) {}

template <typename T>
void trie<T>::node::add_child(const std::pair<char, std::shared_ptr<node>>& key_and_node, const bool is_key)
{
	key_and_node.second->is_key_ = true;
	++size_;
	children_.insert(key_and_node);
}

template <typename T>
size_t trie<T>::node::remove_child(char key)
{
	const size_t size = children_.at(key)->size_;
	children_.erase(key);
	return size;
}

template <typename T>
bool trie<T>::node::is_child(const char key) const
{
	return children_.count(key);
}

template <typename T>
bool trie<T>::node::is_value() const
{
	return is_key_;
}

template <typename T>
std::pair<const char, typename trie<T>::value_type>& trie<T>::node::operator*()
{
	return key_and_value_;
}

template <typename T>
std::pair<const char, typename trie<T>::value_type>* trie<T>::node::operator->()
{
	return &key_and_value_;
}

template <typename T>
bool trie<T>::node::have_parent() const
{
	return (parent_.lock() != nullptr);
}

template <typename T>
void trie<T>::node::set_value(const value_type& value)
{
	is_key_ = true;
	key_and_value_.second = value;
}

template <typename T>
bool trie<T>::node::is_key() const
{
	return is_key_;
}

template <typename T>
std::weak_ptr<typename trie<T>::node> trie<T>::node::get_parent() const
{
	return parent_;
}

template <typename T>
std::shared_ptr<typename trie<T>::node> trie<T>::node::get_child(const char key)
{
	if (children_.count(key))
	{
		return children_.at(key);
	}
	throw node_exception("out_of_range");
}

template <typename T>
auto trie<T>::node::find(const char key)
{
	return children_.find(key);
}

template <typename T>
bool trie<T>::node::empty() const noexcept
{
	return children_.empty();
}

template <typename T>
typename std::map<char, std::shared_ptr<typename trie<T>::node>>::iterator trie<T>::node::begin() noexcept
{
	return children_.begin();
}

template <typename T>
typename std::map<char, std::shared_ptr<typename trie<T>::node>>::iterator trie<T>::node::end() noexcept
{
	return children_.end();
}

template <typename T>
typename std::map<char, std::shared_ptr<typename trie<T>::node>>::const_iterator trie<T>::node::begin() const noexcept
{
	return children_.begin();
}

template <typename T>
typename std::map<char, std::shared_ptr<typename trie<T>::node>>::const_iterator trie<T>::node::end() const noexcept
{
	return children_.end();
}

template <typename T>
template <bool IsConst>
std::weak_ptr<typename trie<T>::node> trie<T>::basic_trie_iterator<IsConst>::get_node()
{
	return ptr_;
}

template <typename T>
template <bool IsConst>
std::weak_ptr<typename trie<T>::node> trie<T>::basic_trie_iterator<IsConst>::get_node() const
{
	return ptr_;
}

template <typename T>
template <bool IsConst>
trie<T>::basic_trie_iterator<IsConst>::basic_trie_iterator(trie& my_trie, std::shared_ptr<node> node) : my_trie_(my_trie), ptr_(node) {}

template <typename T>
template <bool IsConst>
trie<T>::basic_trie_iterator<IsConst>::basic_trie_iterator(trie& my_trie, const basic_trie_iterator& other) : my_trie_(my_trie), ptr_(other.ptr_) {}

template <typename T>
template <bool IsConst>
typename trie<T>:: template basic_trie_iterator<IsConst>& trie<T>::basic_trie_iterator<IsConst>::operator++()
{
	if (ptr_.expired())
	{
		throw std::exception("iterator invalid");
	}

	std::shared_ptr<node> shared = ptr_.lock();

	if (!shared->empty())
	{
		auto next_node_ptr = shared->begin()->second;
		ptr_.reset();
		ptr_ = next_node_ptr;
		return *this;
	}

	//going up
	//if we already in root - return end
	if (shared == my_trie_.root_)
	{
		ptr_.reset();
		//ptr_ = const_cast<trie&>(my_trie_).end().ptr_;
		ptr_ = my_trie_.end().ptr_;
		return *this;
	}

	if (shared->get_parent().expired())
	{
		throw std::exception("parent expired");
	}
	std::shared_ptr<node> parent = shared;

	while (true)
	{
		if (parent->get_parent().expired())
		{
			throw std::exception("ptr expired");
		}
		char key = (**parent).first;

		parent = parent->get_parent().lock();
		if (!parent->have_parent())
		{
			break;
		}
		auto next = ++(parent->find(key));
		if (next != parent->end())
		{
			ptr_.reset();
			ptr_ = (*next).second;
			return *this;
		}
	}

	ptr_.reset();
	ptr_ = my_trie_.end_;
	return *this;
}

template <typename T>
template <bool IsConst>
typename trie<T>::template basic_trie_iterator<IsConst> trie<T>::basic_trie_iterator<IsConst>::operator++(int)
{
	auto tmp = *this;
	++* this;
	return tmp;
}

template <typename T>
template <bool IsConst>
bool trie<T>::basic_trie_iterator<IsConst>::operator==(const basic_trie_iterator& rhs) const
{
	if (!ptr_.expired() && !rhs.ptr_.expired())
	{
		return (ptr_.lock() == rhs.ptr_.lock());
	}

	return false;
}

template <typename T>
template <bool IsConst>
bool trie<T>::basic_trie_iterator<IsConst>::operator!=(const basic_trie_iterator& rhs) const
{
	return !(*this == rhs);
}

template <typename T>
template <bool IsConst>
typename trie<T>::template basic_trie_iterator<IsConst>& trie<T>::basic_trie_iterator<IsConst>::operator=(const basic_trie_iterator& other)
{
	ptr_.reset();
	ptr_ = other.ptr_;

	return *this;
}

template <typename T>
template <bool IsConst>
std::pair<const char, typename trie<T>::template basic_trie_iterator<IsConst>::value_type>& trie<T>::basic_trie_iterator<IsConst>::operator*()
{
	std::weak_ptr<node> current_node = get_node();
	if (current_node.expired())
	{
		throw std::exception{ "node expired" };
	}

	return (**current_node.lock());
}

template <typename T>
template <bool IsConst>
std::pair<const char, typename trie<T>::template basic_trie_iterator<IsConst>::value_type>* trie<T>::basic_trie_iterator<IsConst>::operator->()
{
	const std::weak_ptr<node> current_node = get_node();
	if (current_node.expired())
	{
		throw std::exception{ "node expired" };
	}
	return &(**current_node.lock());
}

template <typename T>
template <bool IsConst>
typename trie<T>::key_type trie<T>::basic_trie_iterator<IsConst>::get_full_key() const
{
	std::string full_key;
	std::shared_ptr<node> current_node = get_node().lock();
	while (current_node->have_parent())
	{
		full_key.push_back((**current_node).first);
		current_node = current_node->get_parent().lock();
	}
	std::reverse(full_key.begin(), full_key.end());
	return full_key;
}

template <typename T>
template <bool IsConst>
bool trie<T>::basic_trie_iterator<IsConst>::is_key() const
{
	const std::weak_ptr<node> current_node = get_node();
	return current_node.lock()->is_key();
}

template <typename T>
template <bool IsConst>
size_t trie<T>::basic_trie_iterator<IsConst>::reset()
{
	auto child_node = ptr_.lock();
	auto parent_node = child_node->get_parent().lock();
	return  parent_node->remove_child((**child_node).first);
}
