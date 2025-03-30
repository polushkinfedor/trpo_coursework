#pragma once
#include <iostream>
#include <vector>
#include <unordered_set>
#include <filesystem>
#include <fstream>

// ����� � 2D-������������
struct node {
    size_t x = std::string::npos;
    size_t y = std::string::npos;
    node() {};
    node(size_t set_x, size_t set_y) : x(set_x), y(set_y) {};
    void set_cords(size_t set_x, size_t set_y) {
        this->x = set_x; 
        this->y = set_y;
    }

    void operator=(const node& other) {
        this->x = other.x;
        this->y = other.y;
    }

    bool operator==(const node& other) {
        return (this->x == other.x && this->y == other.y);
    }
    bool operator!=(const node& other) {
        return (this->x != other.x || this->y != other.y);
    }
    void operator++(void) {
        this->x++;
        this->y++;
    }
};
// ������� ���� �������
enum class el_type {
    START = (int)'S', AVAILABLE = (int)'.', UNAVAILABLE = (int)'X', END = (int)'O'
};
using enum el_type;
using map_type = std::vector<std::vector<el_type>>;
using map_line = std::vector<el_type>;
const std::unordered_set<el_type> availible_el = { START , AVAILABLE , UNAVAILABLE , END };

/// <summary>
/// ��������: ������� �����, ��� ��������� ����� ��������� �����������
/// </summary>
/// <param name="map_file"> ���� �� �����</param>
/// <returns>����� ��������� ����������� � ����� ������</returns>
inline std::pair<map_type,node> map_parsing (const std::filesystem::path& map_file) {
    node start_node;
    map_type map_array;
    std::ifstream map_stream;

    // ������� �������� �����
    map_stream.open(map_file);
    if (not(map_stream)) throw "File unavailible";

    size_t cur_y  = 0;        // ������� ���������� �� Y
    size_t start_counter = 0; // ������� ���������� ����������� ������� �� �����
    size_t end_counter   = 0; // ������� ���������� ����������� �������� ����� ��������
    while (not map_stream.eof()) {
        std::string curr_line; // ������ �����, ���������� �� ������
        map_line cur_map_line; // ������ �����, ����������� � ���� map_line
        std::getline(map_stream, curr_line);

        size_t cur_x = 0; // ������� ���������� �� X
        for (cur_x; cur_x < curr_line.length(); ++cur_x) {
            // �������������� char �������� � ������� �������
            el_type map_element = (el_type)curr_line[cur_x];
            // �������� �� ���������� ������ �������� ��������
            if (not availible_el.contains(map_element)) throw "Map isn't correct";

            if (map_element == START) { // ������ ���������� ������ ����
                start_counter++;
                start_node.set_cords(cur_x, cur_y);
            }
            if (map_element == END) {
                end_counter++;
            }
            cur_map_line.push_back(map_element);
        }
        map_array.push_back(cur_map_line);
        cur_y++;
    }
    // � ����� �� ������ ���� ������ ��� ������ ������ ������ � ����� ��������
    if (start_counter != 1 || end_counter != 1) 
        throw "the map does not have the possibility of finding a deterministic route";
    
    return { map_array, start_node }; // ���������� ����� � ����� ������
}