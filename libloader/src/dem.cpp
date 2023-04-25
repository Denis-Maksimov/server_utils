#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cstdlib>
#include <cstdio>

std::vector<std::string> get_symbols(const std::string& library_path) {
    std::vector<std::string> symbols;
    std::string command = "nm -gC " + library_path;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        std::cerr << "Error: failed to run nm command" << std::endl;
        return symbols;
    }
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        char* name = nullptr;
        char* demangled_name = nullptr;
        char type = '\0';
        if (sscanf(buffer, "%*x %c %ms %ms", &type, &name, &demangled_name) == 3 && type == 'T' && demangled_name != nullptr) {
            symbols.push_back(demangled_name);
            free(demangled_name);
        } else if (sscanf(buffer, "%*x %c %ms", &type, &name) == 2 && type == 'T') {
            symbols.push_back(name);
        }
        free(name);
    }
    return symbols;
}

int main() {
    // путь к библиотеке
    const std::string library_path = "/path/to/library.so";
    
    // чтение символов из библиотеки и деманглинг имен функций
    std::vector<std::string> symbols = get_symbols(library_path);
    for (const auto& symbol : symbols) {
        std::cout << "Function: " << symbol << std::endl;
    }
    
    return 0;
}