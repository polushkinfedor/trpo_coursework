#pragma once
#include "user_stack.hpp"
#include "map_parsing.hpp"

// Стороны обхода алгоритма DFS
enum class sides {
	left, down, right, up, no_side
};
using enum sides;

/// <summary>
/// Описание: Шаг алгоритма DFS (depths-first search). 
/// Порядок обхода:
/// 1. Попытка шага влево
/// 2. Попытка шага вниз
/// 3. Попытка шага вправо
/// 4. Попытка шага вверх 
/// </summary>
/// <param name="cur_map"> карта, содержащая доступные перемещения</param>
/// <param name="solution"> текущее решение</param>
/// <param name="solution_find"> флаг удостоверяющий о нахождении решения</param>
void dfs_step(const map_type& cur_map, TStack<node>& solution, bool& solution_find) {
	const std::vector<sides> crawl_procedure = { left , down , right , up }; // Порядок обхода массива по dfs
	for (const auto& side : crawl_procedure) {
		if (solution_find == true) continue;

		// Вычисление следующей позиции на карте
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

		// Проверка валидности новой координаты
		if (next_step.y < cur_map.size() && next_step.x < cur_map[next_step.y].size()
			and not (solution.contain(next_step))) {
			el_type next_el = cur_map[next_step.y][next_step.x];
			if (next_el == AVAILABLE) { // При доступности продолжаем обход
				solution.push(next_step);
				dfs_step(cur_map, solution, solution_find);
			}
			else if (next_el == END) { // При окончании разворачиваем рекурсию
				solution.push(next_step);
				solution_find = true;
			}
		}

		if (solution_find) return; // Условие разворота рекурсии
	}
	// Удаление элемента из списка решения при невозвожности дальнейшего обхода в глубину
	solution.pop(); 
	return;
}

/// <summary>
/// Описание: Алгоритм поиска в глубину для карты из файла (DFS)
/// </summary>
/// <param name="map_path"> путь до файла, содержащего карту</param>
/// <returns>решение задачи нахождения пути</returns>
TStack<node> dfs(std::filesystem::path map_path) {
	// Получение карты из файла
	std::pair<map_type, node> map_parsing_res = map_parsing(map_path);
	map_type cur_map = map_parsing_res.first;
	node start_node = map_parsing_res.second;

	// Поиск решения
	TStack<node> solution(cur_map.size());
	solution.push(start_node);
	bool solution_find = false;
	dfs_step(cur_map, solution, solution_find);

	// Если решения не существует, выбрасываем исключение
	if (not solution_find) throw "Solution not found"; 
	else return solution;
}

/// <summary>
/// Описание: Алгоритм поиска в глубину для заданной карты (DFS)
/// </summary>
/// <param name="map_area">карта, содержащая доступные перемещения</param>
/// <returns>решение задачи нахождения пути</returns>
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