#pragma once
#include <iostream>
#include <vector>
#include <unordered_set>
#include <filesystem>
#include <fstream>

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
                el_type map_element = (el_type)(std::toupper(curr_line[cur_x]));
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

