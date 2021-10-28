#include "trie.h"
#include <array>

template <typename T>
void print_container(T arg)
{
	for (auto elem : arg)
	{

	}
}



int  main()
{
	trie<int> my_trie;
	my_trie.insert("abc", 3);
	my_trie.insert("ab", 2);
	my_trie.insert("a", 1);
	my_trie.insert("abcd", 4);

	trie<int> my_trie2{ my_trie };

	trie<int> my_trie3{ std::move(my_trie) };
	return 0;
}