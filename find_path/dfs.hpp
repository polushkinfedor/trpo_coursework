#pragma once
#include "user_stack.hpp"
#include "map_parsing.hpp"

// ������� ������ ��������� DFS
enum class sides {
	left, down, right, up, no_side
};
using enum sides;

/// <summary>
/// ��������: ��� ��������� DFS (depths-first search). 
/// ������� ������:
/// 1. ������� ���� �����
/// 2. ������� ���� ����
/// 3. ������� ���� ������
/// 4. ������� ���� ����� 
/// </summary>
/// <param name="cur_map"> �����, ���������� ��������� �����������</param>
/// <param name="solution"> ������� �������</param>
/// <param name="solution_find"> ���� �������������� � ���������� �������</param>
void dfs_step(const map_type& cur_map, TStack<node>& solution, bool& solution_find) {
	const std::vector<sides> crawl_procedure = { left , down , right , up }; // ������� ������ ������� �� dfs
	for (const auto& side : crawl_procedure) {
		if (solution_find == true) continue;

		// ���������� ��������� ������� �� �����
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

		// �������� ���������� ����� ����������
		if (next_step.y < cur_map.size() && next_step.x < cur_map[next_step.y].size()
			and not (solution.contain(next_step))) {
			el_type next_el = cur_map[next_step.y][next_step.x];
			if (next_el == AVAILABLE) { // ��� ����������� ���������� �����
				solution.push(next_step);
				dfs_step(cur_map, solution, solution_find);
			}
			else if (next_el == END) { // ��� ��������� ������������� ��������
				solution.push(next_step);
				solution_find = true;
			}
		}

		if (solution_find) return; // ������� ��������� ��������
	}
	// �������� �������� �� ������ ������� ��� ������������� ����������� ������ � �������
	solution.pop(); 
	return;
}

/// <summary>
/// ��������: �������� ������ � ������� ��� ����� �� ����� (DFS)
/// </summary>
/// <param name="map_path"> ���� �� �����, ����������� �����</param>
/// <returns>������� ������ ���������� ����</returns>
TStack<node> dfs(std::filesystem::path map_path) {
	// ��������� ����� �� �����
	std::pair<map_type, node> map_parsing_res = map_parsing(map_path);
	map_type cur_map = map_parsing_res.first;
	node start_node = map_parsing_res.second;

	// ����� �������
	TStack<node> solution(cur_map.size());
	solution.push(start_node);
	bool solution_find = false;
	dfs_step(cur_map, solution, solution_find);

	// ���� ������� �� ����������, ����������� ����������
	if (not solution_find) throw "Solution not found"; 
	else return solution;
}

/// <summary>
/// ��������: �������� ������ � ������� ��� �������� ����� (DFS)
/// </summary>
/// <param name="map_area">�����, ���������� ��������� �����������</param>
/// <returns>������� ������ ���������� ����</returns>
TStack<node> dfs(std::pair<map_type, node> map_area) {
	map_type cur_map = map_area.first;
	node start_node = map_area.second;

	TStack<node> solution(cur_map.size());
	solution.push(start_node);
	bool solution_find = false;
	dfs_step(cur_map, solution, solution_find);

	if (not solution_find) throw "Solution not found";
	else return solution;
}