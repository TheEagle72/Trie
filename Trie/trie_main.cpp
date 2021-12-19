#include "trie.h"
#include <iostream>

int  main()
{

	trie<int> my_trie;
	my_trie.insert_or_assign("ab", 2);
	my_trie.insert_or_assign("ac", 10);
	my_trie.insert_or_assign("acde", 1);
	my_trie.insert_or_assign("acf", 4);

	auto it = my_trie.begin();
	
	while (it != my_trie.end())
	{
		if (it.is_key())
		{
			std::cout << it->first << std::endl;
			std::cout << it->second << std::endl;
		}
		++it;
	}

	//my_trie.erase(my_trie.find("ac"));
	
    trie<int> my_trie2{ my_trie };
	trie<int> my_trie3{ std::move(my_trie) };

	return 0;
}