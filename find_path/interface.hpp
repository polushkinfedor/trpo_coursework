#include <iostream>
#include <conio.h>
#include <Windows.h>
#include "dfs.hpp"

constexpr int bigM_ASCII = 0x4d;
constexpr int bigP_ASCII = 0x50;
constexpr int skip_file = 2;
#pragma region Signatures
enum menu_point {
    zero = 0x30, one, two, three, four, five, six, manual = 0x6d, print = 0x70
};
using enum menu_point;

enum colors
{
    green = 10, purple = 13, blue = 11, red = 12, yellow = 14, white = 15
};

void menu();
static void print_map(map_type& current_map, TStack<node> sln, std::ostream& out = std::cout);
static void exit_message(bool& exit_flag);
static void incorrect_data_message(const char err[]);
static void incorrect_file_message();
static void print_manual();
static bool check_file_name(const std::string& file_name);
static bool check_file_exist(std::string& file_name);
static void set_color(std::string, int, std::ostream& out = std::cout);
static std::filesystem::path find_file_menu(const std::filesystem::path& directory);
static int file_open_menu(std::string filename);
static void map_load_tf(map_type& exemple);
static void sln_load_tf(TStack<node>& sln);
#pragma endregion

//--------------------------- functional program units ---------------------------------

void menu() {
    // Function responsible for user interaction with the program
    bool end_programm = false;
    map_type current_map = { {START,       AVAILABLE,   AVAILABLE}, 
                             {UNAVAILABLE, AVAILABLE,   AVAILABLE}, 
                             {UNAVAILABLE, UNAVAILABLE, END      } };
    node curr_start = { 0, 0 };

    // Variables for work with files
    std::filesystem::path file_path; 
    std::ifstream file_stream;

    std::pair<map_type&, node&> start_info = { current_map , curr_start };
    type_procedure proc_type = clockwise;
    TStack<node> sln;

    while (not(end_programm)) {
        system("cls");
        set_color("You are working with programm, that finding path from S to O, " \
            "excluding X symbols.\n", colors::white);
        print_map(start_info.first, sln);
        set_color("current type of procedure (clockwise/counterclockwise/random):\n", colors::blue);
        switch (proc_type)
        {
        case type_procedure::clockwise:
            std::cout << "clockwise\n";
            break;
        case type_procedure::counterclockwise:
            std::cout << "counterclockwise\n";
            break;
        case type_procedure::random:
            std::cout << "random\n";
            break;
        default:
            break;
        }
        std::cout << "\n\n";

        std::cout << "To find solution:                  "; set_color("enter -- 1\n", colors::green);
        std::cout << "To enter map of area from console: "; set_color("enter -- 2\n", colors::green);
        std::cout << "To enter map of area from file:    "; set_color("enter -- 3\n", colors::green);
        std::cout << "To copy map of area to file:       "; set_color("enter -- 4\n", colors::green);
        std::cout << "To clear map of area:              "; set_color("enter -- 5\n", colors::green);
        std::cout << "To change type_procedure:          "; set_color("enter -- 6\n\n", colors::green);
        std::cout << "To print solution vector:          "; set_color("enter -- p\n", colors::green);
        std::cout << "To open manual:                    "; set_color("enter -- m\n", colors::green);
        std::cout << "To exit "; set_color("enter -- 0\n\n", colors::green);


        int user_enter;
        bool exit_flag = false;
        while (!exit_flag) {
            user_enter = _getch();
            // if find 'M' or 'P' translate to lower register
            user_enter = (user_enter == bigM_ASCII) ? manual : user_enter;
            user_enter = (user_enter == bigP_ASCII) ? print : user_enter;
            exit_flag = true;
            switch ((menu_point)user_enter) {
            case one:
                system("cls");
                try {
                    sln = dfs(start_info, proc_type);
                    sln.invert();
                    print_map(start_info.first, sln);
                    set_color("\nVector of solution:\n", colors::blue);
                    while (not sln.empty())
                    {
                        std::cout << "(" << sln.top().x << "," << sln.top().y << ")\n";
                        sln.pop();
                    }
                }
                catch (const char e[]) {
                    std::cout << e;
                }
                _getch();
                break;
            case two:
                system("cls");
                std::cout << "Enter the map (X = wall, . = path, O = end, S = start).\n";
                set_color("Type /end to finish input.\n", green);
                try {
                    start_info = map_parsing(std::cin);
                }
                catch (const char* e) {
                    incorrect_data_message(e);
                }
                break;
            case three:
                system("cls");
                file_path = find_file_menu(std::filesystem::current_path());
                if (file_path != "") {
                    file_stream.open(file_path);
                    try {
                        start_info = map_parsing(file_stream);
                    }
                    catch (const char* e) {
                        incorrect_data_message(e);
                    }
                    file_stream.close();
                }
                break;
            case four:
                map_load_tf(start_info.first);
                break;
            case five:
                start_info.first = { {} };
                start_info.second = {};
                break;
            case six:
                if (proc_type == clockwise)             proc_type = counterclockwise;
                else if (proc_type == counterclockwise) proc_type = random;
                else if (proc_type == random)           proc_type = clockwise;
                break;
            case manual:
                print_manual();
                break;
            case print:
                try {
                    sln = dfs(start_info, proc_type);
                    sln.invert();
                    sln_load_tf(sln);
                }
                catch (const char e[]) {
                    std::cout << e;
                }
                break;
            case zero: // exit from func
                exit_message(end_programm);
                break;
            default:
                exit_flag = false;
                break; // skip unregulated user commands
            }
        }
    }
}

static void print_map(map_type& current_map, TStack<node> sln, std::ostream& out) {
    set_color("Current map of area:\n", colors::blue);
    if (current_map[0].size() == NULL) {
        out << "CLEAR \n";
    }
    else {
        for (size_t y = 0; y < current_map.size(); ++y) {
            for (size_t x = 0; x < current_map[y].size(); ++x) {
                std::string printing_line = " ";
                printing_line[0] = (char)current_map[y][x];
                if (sln.contain({ x, y })) {
                    set_color(printing_line, colors::green, out);
                }
                else {
                    set_color(printing_line, colors::white, out);
                }
            } out << "\n";
        }
    }
}

static void set_color(std::string text, int color, std::ostream& out) { // выбор цвета (в значение color подставлять из перечисления serv_colors)
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    out << text;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

//---------------------------- informational program units --------------------------------

static void exit_message(bool& exit_flag) {
    // confirmation message before exiting the program
    bool message_reload = true;
    while (message_reload) {

        system("cls");
        std::cout << "Are you shure, that you want exit?\n";
        std::cout << "Y/n\n";

        char user_answer = _getch();
        user_answer = std::tolower(user_answer);
        if (user_answer == 'y') {
            exit_flag = true;
            message_reload = false;
        }
        else if(user_answer == 'n') {
            exit_flag = false;
            message_reload = false;
        }
    }
}

static void incorrect_data_message(const char err[]) {
    // Information message about incorrect input data
    set_color("you entered incorrect data\n", colors::red);
    set_color(err, colors::red);
    _getch();
}

static void incorrect_file_message() {
    // Information message about an incorrect file
    std::cout << "file contains incorrect data, please change file data.\n";
    std::cout << "to continue, enter any key    ";
    _getch();
}

static void print_manual() {
    system("cls");
    set_color("MANUAL FOR THE PATHFINDING PROGRAM\n", blue); // Section header

    std::cout << "1. Introduction\n";
    std::cout << "   This program implements a pathfinding algorithm, where the goal is to find a way\n"
                 "   from a start point 'S' to an end point 'O', while avoiding obstacles represented by 'X'.\n\n";

    std::cout << "2. Controls:\n";
    std::cout << "   - You can navigate through the map and choose options by entering corresponding numbers.\n";
    std::cout << "   - The pathfinding algorithm can be modified by choosing the procedure type (Clockwise, Counterclockwise, or Random).\n";
    std::cout << "   - Maps can be manually entered through the console, loaded from a file, or saved to a file.\n\n";

    std::cout << "3. Menu Options:\n";
    std::cout << "   1. Find solution: Starts the pathfinding search using the current map.\n";
    std::cout << "   2. Input map from console: Allows you to manually input a new map.\n";
    std::cout << "   3. Load map from file: Opens an existing map from a file and loads it.\n";
    std::cout << "   4. Save map to file: Saves the current map to a file.\n";
    std::cout << "   5. Clear map: Clears the current map.\n";
    std::cout << "   6. Change procedure type: Allows you to change the pathfinding algorithm (Clockwise, Counterclockwise, or Random).\n";
    std::cout << "   p. Print solution to file: Print solution for current map to a file.\n";
    std::cout << "   m. Show manual: Displays this manual.\n";
    std::cout << "   0. Exit: Exits the program.\n\n";

    std::cout << "4. How to read the map:\n";
    std::cout << "   - 'S' is the start point.\n";
    std::cout << "   - 'O' is the endpoint.\n";
    std::cout << "   - 'X' represents obstacles (impassable areas).\n";
    std::cout << "   - '.' represents passable areas.\n\n";

    set_color("Press any key to return to the menu.", green);
    _getch(); // Wait for user input before returning to the menu
}


//------------------------------- logical program units ----------------------------------

static bool check_file_name(const std::string& file_name) {
    // Check for invalid characters in the file name
    std::vector<char> bad_symbols = {
          '<',
          '>',
          ':',
          '\"',
          '/' ,
          '\\',
          '|' ,
          '?' ,
          '*' ,
    };
    if (file_name == "") return false;
    for (auto sym : bad_symbols) if (file_name.find(sym) != -1) return false;
    return true;
}

static bool check_file_exist(std::string& file_name) {
    // This function converts the name to the full file path and returns the existence status
    if (file_name.find(".txt") != file_name.length() - 4 or file_name.length() < 4) file_name += ".txt"; // Add extension if not provided by the user
    std::string f_path = std::filesystem::current_path().string();
    f_path += ("\\" + file_name);
    file_name = f_path;

    return std::filesystem::exists(f_path);
}

//--------------------------------------- working with files -----------------------------------------

static bool is_number(const std::string& str) {
    return !str.empty() && (str.find_first_not_of("0123456789") == str.npos);
}

static std::string trim(const std::string& str) {
    auto space = ' ';
    // If the string does not contain spaces to remove
    if ((str.find_first_not_of(space) == str.npos)) {
        return std::string();
    }
    else {
        // Trim spaces at the beginning and end
        size_t count = str.find_last_not_of(space) - str.find_first_not_of(space) + 1;
        return std::string(str.substr(str.find_first_not_of(space), count));
    }
}

static std::filesystem::path correct_ffn(const std::string& str) {
    std::string nstr = trim(str);
    if (not nstr.empty()) {
        if (nstr.back() == '\"' or nstr.back() == '\'') nstr.pop_back();
    }
    if (not nstr.empty()) {
        if (nstr[0] == '\"' or nstr[0] == '\'') nstr = nstr.substr(1);
    }
    std::filesystem::path ffn = std::filesystem::path(nstr);
    if (ffn.is_absolute()) {
        // If nstr is an absolute file path
        return std::filesystem::path(nstr).lexically_normal();
    }
    else if (nstr.find_first_of("\\/:*?\"<>|+ !") != nstr.npos
        or nstr.empty()
        or nstr.back() == L'.'
        or nstr.length() > _MAX_PATH) {
        // If nstr is just a file name
        // and the file name is incorrect
        return std::filesystem::path();
    }
    else {
        // If nstr is a correct file name
        return std::filesystem::path(nstr).lexically_normal();
    }
}

static void find_files(std::string key, const std::filesystem::path& sPath, std::vector<std::filesystem::path>& files) try {
    files.clear();
    std::filesystem::path key_path = correct_ffn(key);
    // Search for the file by name
    if (exists(key_path) and key_path.extension() == ".txt") {
        files.push_back(key_path);
    }
    // Search for files in the specified directory
    else if (exists(key_path) and is_directory(key_path)) {
        for (const auto& var : std::filesystem::directory_iterator(key_path, std::filesystem::directory_options::skip_permission_denied)) {
            if (var.path().extension() == ".txt") {
                // Browse all available files with the file extension
                files.push_back(var.path());
            }
        }
    }
    // Search for files in the default directory by substring
    else if (exists(sPath) and is_directory(sPath)) {
        for (const auto& var : std::filesystem::directory_iterator(sPath, std::filesystem::directory_options::skip_permission_denied)) {
            if (var.path().filename().string().find(key) != key.npos
                and var.path().extension() == ".txt") {
                // Browse all available files with the file extension and keyword
                files.push_back(var.path());
            }
        }
    }
}
catch (...) { // Exception handling
    files.clear();
    std::cout << "Something went wrong, please try again or change the directory\n\n";
}

static std::filesystem::path find_file_menu(const std::filesystem::path& directory) {
    std::vector<std::filesystem::path> files;
    std::string input;
    size_t input_int = 0;

    do {
        std::cout << "Enter the full or partial file name to search for rhymes.\n"
            "To display all available files from the program folder, enter an empty line[Enter].\n"
            "To exit input /exit\n\n";

        std::getline(std::cin, input);
        system("cls");

        find_files(input, directory, files);
        input = trim(input);
        if (input == "/exit") return {};

        if (files.size() == 0ull) {
            std::cout << "Files not found, please try again.\n\n";
        }
        else {
            do {
                std::cout << "Select the file to search for rhymes.\n";
                for (size_t i = 0; i != files.size(); ++i) {
                    std::cout << i + 1 << ". " << files[i].filename() << '\n';
                }
                std::cout << "Enter the file number or the /again command to repeat the file/files search.\n"
                    "To exit input /exit\n\n";

                std::getline(std::cin, input);
                system("cls");

                if (is_number(input)) {
                    input_int = std::stoull(input);

                    // The user entered the correct number from the list
                    if (0 < input_int and input_int <= files.size()) {
                        // If the selected file is empty
                        if (std::filesystem::file_size(files[input_int - 1]) == 0) {
                            std::cout << "The selected file is empty, try again.\n\n";
                        }
                        else {
                            return files[input_int - 1];
                        }
                    }
                    else {
                        std::cout << "Please select a file from the list.\n\n";
                    }
                }
                else if (input == "/again") {
                    // The user wants to search again
                    break;
                }
                else if (input == "/exit") return {};
                else {
                    std::cout << "Incorrect input, please try again.\n\n";
                }
            } while (true);
        }
    } while (true);
}

static int file_open_menu(std::string filename) {
    // Menu for how to open a file to unload information
    int op_code = 0;
    if (std::filesystem::exists(filename)) { // If the file exists
        set_color("Attention! File already exists\n", colors::red);
        std::cout << "Remember, if you want to fill the file, the information in the console will be\n";
        std::cout << "overwritten by the information that will be saved in the file\n\n";

        std::cout << "If you want to recreate the file enter -- 1\n";
        std::cout << "Otherwise, enter -- 0\n";

        while (op_code == 0) {  // Until the user selects one of the options
            int var = _getch();
            switch (var) {
            case one: // Merge data from the console and file with priority for the console
                op_code = 1;
                break;
            case zero: // Recreate the file
                op_code = skip_file;
                break;
            }
        }
    }
    return op_code;
};

static void map_load_tf(map_type& cur_map) {
    // Function responsible for unloading data into a file
    bool need_exit = false;
    while (not(need_exit)) {
        std::string file_name;
        system("cls");
        std::cout << "Specify the name of the file where you are going to save the map\n";
        std::cout << "It will be located in the directories: ";
        set_color(std::filesystem::current_path().string() + "\n", colors::blue);
        set_color("To go back to the menu -- enter 0\n\n", colors::green);
        std::cout << "Please enter the file name with file extension: ";
        std::getline(std::cin, file_name); // Get the file name
        if (file_name == "0") return; // Special exit code as indicated in the interaction window

        trim(file_name);

        if (check_file_name(file_name)) { // If the file name is valid
            check_file_exist(file_name); // Get the full name for the file

            int op_code = file_open_menu(file_name);     // How do we want to open the file if it already exists (1 -- continue filling, 2 -- recreate)
            bool need_load = false;

            std::ofstream file(file_name, std::ios::out);

            if (op_code != skip_file) {
                print_map(cur_map, {}, file);
            }
            file.close();
            need_exit = true;
        }
        else { // If the file name is invalid
            system("cls");
            std::cout << "The entered file name is incorrect, make sure it does not contain the following characters:\n"
                " < >    :    \"    \\    /    |    ?    *\n"
                "To continue, enter any key";
            _getch();
        }
    }
}

static void sln_load_tf(TStack<node>& sln) {
    // Function responsible for unloading data into a file
    bool need_exit = false;
    while (not(need_exit)) {
        std::string file_name;
        system("cls");
        std::cout << "Specify the name of the file where you are going to save vector of solution\n";
        std::cout << "It will be located in the directories: ";
        set_color(std::filesystem::current_path().string() + "\n", colors::blue);
        set_color("To go back to the menu -- enter 0\n\n", colors::green);
        std::cout << "Please enter the file name with file extension: ";
        std::getline(std::cin, file_name); // Get the file name
        if (file_name == "0") return; // Special exit code as indicated in the interaction window

        trim(file_name);

        if (check_file_name(file_name)) { // If the file name is valid
            check_file_exist(file_name); // Get the full name for the file

            int op_code = file_open_menu(file_name);     // How do we want to open the file if it already exists (1 -- continue filling, 2 -- recreate)
            bool need_load = false;

            std::ofstream file(file_name, std::ios::out);

            if (op_code != skip_file) {
                set_color("Vector of solution:\n", colors::blue, file);
                while (not sln.empty())
                {
                    file << "(" << sln.top().x << "," << sln.top().y << ")\n";
                    sln.pop();
                }
            }
            file.close();
            need_exit = true;
        }
        else { // If the file name is invalid
            system("cls");
            std::cout << "The entered file name is incorrect, make sure it does not contain the following characters:\n"
                " < >    :    \"    \\    /    |    ?    *\n"
                "To continue, enter any key";
            _getch();
        }
    }
}