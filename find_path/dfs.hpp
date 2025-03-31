#pragma once
#include "user_stack.hpp"
#include "map_parsing.hpp"
#include <ctime>

// The sides of bypassing the DFS algorithm
enum class sides {
	left, down, right, up, no_side
};
using enum sides;
// Type of porecudure (clockwise, counterclockwise, random)
enum class type_procedure {
	clockwise, counterclockwise, random
};
using enum type_procedure;
constexpr size_t crawl_procedure_len = 3; // max element of crawl_procedure array index

/// <summary>
/// Description: The step of the DFS algorithm (depths-first search).
/// The crawl procedure is randomized
/// </summary>
/// <param name="cur_map">a map containing the available movements</param>
/// <param name="solution">current solution</param>
/// <param name="solution_find">a flag certifying that a solution has been found</param>
/// <param name="proc_type">type of porecudure (clockwise, counterclockwise, random)</param>
void dfs_step(const map_type& cur_map, TStack<node>& solution, bool& solution_find, type_procedure proc_type) {
	std::vector<sides> crawl_procedure = { up , down , left , right };

#pragma region Rules of procedure
	if (proc_type == random) { // Random order
		srand(time(NULL));
		for (size_t index = 0; index < crawl_procedure_len; ++index) {
			size_t random_el_index = rand() % (crawl_procedure_len + 1);
			sides temp_side = crawl_procedure[random_el_index];
			crawl_procedure[random_el_index] = crawl_procedure[index];
			crawl_procedure[index] = temp_side;
		}
	}
	else if (proc_type == counterclockwise) { // Counterclockwise order, started by left direction
		crawl_procedure = { left , up, right , down };
	}
	else if (proc_type == clockwise) { // Clockwise order, started by right direction
		crawl_procedure = { right , down, left , up };
	}
#pragma endregion

	for (const auto& side : crawl_procedure) {
		if (solution_find == true) continue;

		// Calculating the next position on the map
		node next_step;
		switch (side) {
		case left:
			next_step.set_cords(solution.top().x - 1, solution.top().y);
			break;
		case down:
			next_step.set_cords(solution.top().x, solution.top().y - 1);
			break;
		case right:
			next_step.set_cords(solution.top().x + 1, solution.top().y);
			break;
		case up:
			next_step.set_cords(solution.top().x, solution.top().y + 1);
			break;
		default:
			break;
		}

		// Checking the validity of the new coordinate
		if (next_step.y < cur_map.size() && next_step.x < cur_map[next_step.y].size()
			and not (solution.contain(next_step))) {
			el_type next_el = cur_map[next_step.y][next_step.x];
			if (next_el == AVAILABLE) { // If available, continue the crawl
				solution.push(next_step);
				dfs_step(cur_map, solution, solution_find, proc_type);
			}
			else if (next_el == END) { // At the end, we expand the recursion
				solution.push(next_step);
				solution_find = true;
			}
		}

		if (solution_find) return; // Recursion reversal condition
	}
	// Removing an item from the solution list if further depth-first traversal is not possible
	solution.pop();
	return;
}

/// <summary>
/// Description: Depth-first search algorithm for a map from a file (DFS)
/// </summary>
/// <param name="map_path">the path to the file containing the map</param>
/// <param name="proc_type">type of porecudure (clockwise, counterclockwise, random)</param>
/// <returns>solving the problem of finding a path</returns>
TStack<node> dfs(std::filesystem::path map_path, type_procedure proc_type = clockwise) {
	// Getting a map from a file
	std::pair<map_type, node> map_parsing_res = map_parsing(map_path);
	map_type cur_map = map_parsing_res.first;
	node start_node = map_parsing_res.second;

	// Finding a solution
	TStack<node> solution(cur_map.size());
	solution.push(start_node);
	bool solution_find = false;
	dfs_step(cur_map, solution, solution_find, proc_type);

	// If there is no solution, we throw an exception.
	if (not solution_find) throw "Solution not found"; 
	else return solution;
}

/// <summary>
/// Description: Depth-first search Algorithm for a given Map (DFS)
/// </summary>
/// <param name="map_area">map containing the available movements</param>
/// <param name="proc_type">type of porecudure (clockwise, counterclockwise, random)</param>
/// <returns>solving the problem of finding a path</returns>
TStack<node> dfs(std::pair<map_type, node> map_area, type_procedure proc_type = clockwise) {
	map_type cur_map = map_area.first;
	node start_node = map_area.second;

	TStack<node> solution(cur_map.size());
	solution.push(start_node);
	bool solution_find = false;
	dfs_step(cur_map, solution, solution_find, proc_type);

	if (not solution_find) throw "Solution not found";
	else return solution;
}