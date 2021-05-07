#pragma once
#include <chrono>
#include <iostream>
#include <functional>

template<auto Callback, class SourceType, typename...Params>
static void LogTime(SourceType* instancePointer, Params&&...p) 
{
	auto start = std::chrono::high_resolution_clock::now();

	std::invoke(Callback, instancePointer, std::forward<Params>(p)...);

	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	std::cout << "Function took: " << elapsed.count() << " s to complete." << std::endl;
}


struct DebugTimer {
	void Start(const std::string& aMessage = "")
	{
		std::cout << aMessage << std::endl;
		ourStart = std::chrono::high_resolution_clock::now();
	}

	double End()
	{
		auto finish = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = finish - ourStart;
		std::cout << "Function Took: " << elapsed.count() << " s to complete." << std::endl;
		return elapsed.count();
	}
	std::chrono::steady_clock::time_point ourStart;
};
