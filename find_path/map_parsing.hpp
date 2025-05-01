#pragma once
#include <iostream>
#include <vector>
#include <unordered_set>
#include <filesystem>
#include <fstream>
#include <ctime>

namespace MapSymbolsMask {
    char unav_all = '|';
    char unav_hor = '_';
    char unav_ver = '|';
    char av = '.';
    char ed = 'F';
    char st = 'S';
    char error_data = 'e';
};

// A point in 2D space
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
    void operator++(void) {
        this->x++;
        this->y++;
    }
};
// Map body element
enum class el_type {
    START = (int)'S', AVAILABLE = (int)'.', UNAVAILABLE = (int)'X', END = (int)'O'
};
using enum el_type;
using map_type = std::vector<std::vector<el_type>>;
using map_line = std::vector<el_type>;
const std::unordered_set<el_type> availible_el = { START , AVAILABLE , UNAVAILABLE , END };

/// <summary>
/// Description: parsing a stream to get a map of available movements
/// </summary>
/// <param name="map_stream">stream with the map of area</param>
/// <returns>map of available movements and starting point</returns>
inline std::pair<map_type,node> map_parsing (std::istream& map_stream)  {
    node start_node;
    map_type map_array;

    size_t cur_y  = 0;        // The current Y coordinate
    size_t start_counter = 0; // Counter for the number of starts encountered on the map
    size_t end_counter = 0;   // Counter for the number of completed route points encountered
    bool end_flag = false;
    if (not map_stream) throw "File dont exist";
    while (not map_stream.eof() and not end_flag) {
        std::string curr_line; // The map string received from the stream
        map_line cur_map_line; // A map line converted to the map_line type
        std::getline(map_stream, curr_line);
        if (curr_line != "/end") {
            size_t cur_x = 0; // The current X coordinate
            for (cur_x; cur_x < curr_line.length(); ++cur_x) {
                // Converting a char element to a matrix elements
                el_type map_element;
                if (curr_line[cur_x] == MapSymbolsMask::unav_all or
                    curr_line[cur_x] == MapSymbolsMask::unav_hor or
                    curr_line[cur_x] == MapSymbolsMask::unav_ver) {
                    map_element = UNAVAILABLE;
                }
                else if (curr_line[cur_x] == MapSymbolsMask::av) {
                    map_element = AVAILABLE;
                }
                else if (curr_line[cur_x] == MapSymbolsMask::st) {
                    map_element = START;
                }
                else if (curr_line[cur_x] == MapSymbolsMask::st) {
                    map_element = END;
                }
                else {
                    map_element = (el_type)MapSymbolsMask::error_data;
                }
                
                // Checking for the content of valid characters only
                if (not availible_el.contains(map_element)) throw "Map isn't correct";

                if (map_element == START) { // Find out the coordinates of the beginning of the path
                    start_counter++;
                    start_node.set_cords(cur_x, cur_y);
                }
                if (map_element == END) {
                    end_counter++;
                }
                cur_map_line.push_back(map_element);
            }
            // The map should not contain more or less than one beginning and end of the route.
            if (start_counter > 1 || end_counter > 1)
                throw "the map does not have the possibility of finding a deterministic route";
            map_array.push_back(cur_map_line);
            cur_y++;
        }
        else {
            end_flag = true;
        }
    }
    if (start_counter != 1 || end_counter != 1)
        throw "the map does not have the possibility of finding a deterministic route";
    return { map_array, start_node }; // return the map and the starting point
}

static inline void division_step(map_type& cur_map, const std::pair<size_t, size_t>& map_size, const unsigned int seed) {
    auto gen_sub_map = [seed](map_type& cur_map, const std::pair<size_t, size_t>& map_size, 
                          size_t hor_wall_pos, size_t ver_wall_pos) {
            size_t x_size = map_size.second;
            size_t y_size = map_size.first;

            map_type sub_map1;
            map_type sub_map2;
            map_type sub_map3;
            map_type sub_map4;
            // left-upper corner
            if (hor_wall_pos < INT32_MAX and ver_wall_pos < INT32_MAX) {
                sub_map1.resize(hor_wall_pos, map_line(ver_wall_pos));
            }
            // left-lower corner
            if ((y_size - hor_wall_pos - 1) < INT32_MAX and ver_wall_pos < INT32_MAX) {
                sub_map2.resize((int)y_size - (int)hor_wall_pos - 1, map_line(ver_wall_pos));
            }
            // right-upper corner
            if (hor_wall_pos < INT32_MAX and (x_size - ver_wall_pos - 1) < INT32_MAX) {
                sub_map3.resize(hor_wall_pos, map_line(x_size - ver_wall_pos - 1));
            }
            // right-lower corner
            if ((y_size - hor_wall_pos - 1) < INT32_MAX and (x_size - ver_wall_pos - 1) < INT32_MAX) {
                sub_map4.resize(y_size - hor_wall_pos - 1, map_line((int)x_size - (int)ver_wall_pos - 1));
            }

            for (size_t x = 0; x < x_size; ++x) {
                for (size_t y = 0; y < y_size; ++y) {
                    if (x < ver_wall_pos and y < hor_wall_pos)
                        sub_map1[y][x] = cur_map[y][x];
                    else if (x < ver_wall_pos and y > hor_wall_pos)
                        sub_map2[y - hor_wall_pos - 1][x] = cur_map[y][x];
                    else if (x > ver_wall_pos and y < hor_wall_pos)
                        sub_map3[y][x - ver_wall_pos - 1] = cur_map[y][x];
                    else if (x > ver_wall_pos and y > hor_wall_pos)
                        sub_map4[y-hor_wall_pos - 1][x-ver_wall_pos - 1] = cur_map[y][x];
                }
            }

            if (not sub_map1.empty()) {
                division_step(sub_map1, { sub_map1.size(), sub_map1[0].size() }, seed);
            }
            if (not sub_map2.empty()) {
                division_step(sub_map2, { sub_map2.size(), sub_map2[0].size() }, seed);
            }
            if (not sub_map3.empty()) {
                division_step(sub_map3, { sub_map3.size(), sub_map3[0].size() }, seed);
            }
            if (not sub_map4.empty()) {
                division_step(sub_map4, { sub_map4.size(), sub_map4[0].size() }, seed);
            }

            for (size_t x = 0; x < x_size; ++x) {
                for (size_t y = 0; y < y_size; ++y) {
                    if (x < ver_wall_pos and y < hor_wall_pos)
                        cur_map[y][x] = sub_map1[y][x];
                        if (x == ver_wall_pos - 1 and (cur_map[y][x + 1] == AVAILABLE and cur_map[y][x] == UNAVAILABLE))
                            cur_map[y][x] = AVAILABLE;
                        if (y == hor_wall_pos - 1 and (cur_map[y+1][x] == AVAILABLE and cur_map[y][x] == UNAVAILABLE))
                            cur_map[y][x] = AVAILABLE;
                    else if (x < ver_wall_pos and y > hor_wall_pos)
                        cur_map[y][x] = sub_map2[y - hor_wall_pos - 1][x];
                        if (x == ver_wall_pos - 1 and (cur_map[y][x + 1] == AVAILABLE and cur_map[y][x] == UNAVAILABLE))
                            cur_map[y][x] = AVAILABLE;
                        if (y == hor_wall_pos + 1 and (cur_map[y - 1][x] == AVAILABLE and cur_map[y][x] == UNAVAILABLE))
                            cur_map[y][x] = AVAILABLE;
                    else if (x > ver_wall_pos and y < hor_wall_pos)
                        cur_map[y][x] = sub_map3[y][x - ver_wall_pos - 1];
                        if (x == ver_wall_pos + 1 and (cur_map[y][x - 1] == AVAILABLE and cur_map[y][x] == UNAVAILABLE))
                            cur_map[y][x] = AVAILABLE;
                        if (y == hor_wall_pos - 1 and (cur_map[y + 1][x] == AVAILABLE and cur_map[y][x] == UNAVAILABLE))
                            cur_map[y][x] = AVAILABLE;
                    else if (x > ver_wall_pos and y > hor_wall_pos)
                        cur_map[y][x] = sub_map4[y - hor_wall_pos - 1][x - ver_wall_pos - 1];
                        if (x == ver_wall_pos + 1 and (cur_map[y][x - 1] == AVAILABLE and cur_map[y][x] == UNAVAILABLE))
                            cur_map[y][x] = AVAILABLE;
                        if (y == hor_wall_pos + 1 and (cur_map[y - 1][x] == AVAILABLE and cur_map[y][x] == UNAVAILABLE))
                            cur_map[y][x] = AVAILABLE;
                }
            }
        };

    srand(seed);
    bool is_second_door_horizontal = rand()%2;
    bool generate_hor_wall = map_size.first  > 2;
    bool generate_ver_wall = map_size.second > 2;
    if (not generate_hor_wall || not generate_ver_wall) return;

    
    size_t hor_wall_pos = (generate_hor_wall) ? (rand() % (map_size.first-2)) + 1 : 0;
    size_t ver_wall_pos = (generate_ver_wall) ? (rand() % (map_size.second-2)) + 1 : 0;
    if (generate_hor_wall) { // Create horizontal wall and door
        for (size_t x = 0; x < map_size.second; ++x) {
            cur_map[hor_wall_pos][x] = UNAVAILABLE;
        }
        size_t hor_door_pos1 = (rand() % (ver_wall_pos));
        size_t hor_door_pos2 = ((rand() % (map_size.second - ver_wall_pos-1)) + ver_wall_pos) + 1;
        if (not is_second_door_horizontal) {
            if (rand() % 2 == 0) {
                cur_map[hor_wall_pos][hor_door_pos1] = AVAILABLE;
            }
            else {
                cur_map[hor_wall_pos][hor_door_pos2] = AVAILABLE;
            }
        }
        else {
            cur_map[hor_wall_pos][hor_door_pos1] = AVAILABLE;
            cur_map[hor_wall_pos][hor_door_pos2] = AVAILABLE;
        }
    }

    if (generate_ver_wall) { // Create vertical wall and door
        size_t ver_door_pos1 = (rand() % (hor_wall_pos));
        size_t ver_door_pos2 = (rand() % (map_size.first - hor_wall_pos-1)) + hor_wall_pos + 1;
        for (size_t y = 0; y < map_size.first; ++y) {
            cur_map[y][ver_wall_pos] = UNAVAILABLE;
        }
        if (is_second_door_horizontal) {
            if (rand() % 2 == 0) {
                cur_map[ver_door_pos1][ver_wall_pos] = AVAILABLE;
            }
            else {
                cur_map[ver_door_pos2][ver_wall_pos] = AVAILABLE;
            }
        }
        else {
            cur_map[ver_door_pos1][ver_wall_pos] = AVAILABLE;
            cur_map[ver_door_pos2][ver_wall_pos] = AVAILABLE;
        }
    }
    gen_sub_map(cur_map, { cur_map.size(), cur_map[0].size() }, hor_wall_pos, ver_wall_pos);
}

inline std::pair<map_type, node> rand_map_generate(const unsigned int seed, const std::pair<size_t, size_t>& map_size = { 10, 10 }) {
    map_type random_map;
    for (size_t x = 0; x < map_size.second; ++x) {
        map_line base_line;
        for (size_t y = 0; y < map_size.first; ++y) {
            base_line.push_back(AVAILABLE);
        }
        random_map.push_back(base_line);
    }
    division_step(random_map, { random_map.size(), random_map[0].size() }, seed);
    
    random_map[0][0] = START;
    random_map[map_size.first-1][map_size.second-1] = END;
    return { random_map, { 0,0 } };
}
