#pragma once
#include <map>
#include <string>
#include <iostream>
//#define PRINT_HASH_DEBUG
namespace Hasher
{
	inline size_t GetHashValue(const std::string& aString)
	{
		std::hash<std::string> hasher;
#ifdef _DEBUG
#ifdef PRINT_HASH_DEBUG
		std::cout << aString << ", hashvalue: " << hasher(aString) << "                " << __FUNCTION__ << std::endl;
#endif
#endif
		return hasher(aString);
	}
}