#include <iostream>
#include "dfs.hpp"

int main()
{
	try {
		TStack<node> solution = dfs("C:\\Study\\TRPO_CW\\find_path\\find_path\\map.txt");
		TStack<node> solution_ltor(solution.size());
		while (not solution.empty())
		{
			node step = solution.pop();
			solution_ltor.push(step);
		}
		while (not solution_ltor.empty())
		{
			std::cout << "(" << solution_ltor.top().x << "," << solution_ltor.top().y << ")\n";
			solution_ltor.pop();
		}
	}
	catch (const char e[]) {
		std::cout << e;
	}
}