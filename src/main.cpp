#include <filesystem>
#include <iostream>
#include <optional>
#include <memory>
#include <vector>
#include <algorithm>

#if defined(_WIN32)
    #define NOMINMAX
    #include <windows.h>
#endif

#include "calc/eval.hpp"
#include "calc/plugins.hpp"

namespace fs = std::filesystem;

static const char* platform_ext() {
#if defined(_WIN32)
    return ".dll";
#elif defined(__APPLE__)
    return ".dylib";
#else
    return ".so";
#endif
}

int main(int argc, char* argv[]) {
    try {
#if defined(_WIN32)
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
#endif

        calc::FunctionRegistry reg;

#if defined(_DEBUG)
        const char* cfg = "Debug";
#else
        const char* cfg = "Release";
#endif

        std::vector<fs::path> dirs;

        fs::path cwd = fs::current_path();
        dirs.push_back(cwd / "plugins");
        dirs.push_back(cwd / "plugins" / cfg);

        if (argc > 0 && argv && argv[0]) {
            try {
                fs::path exe = fs::weakly_canonical(fs::path(argv[0]));
                fs::path exe_dir = exe.parent_path();

                dirs.push_back(exe_dir / "plugins");
                dirs.push_back(exe_dir / "plugins" / cfg);

                dirs.push_back(exe_dir.parent_path() / "plugins");
                dirs.push_back(exe_dir.parent_path() / "plugins" / cfg);
            } catch (...) {
            }
        }

        for (auto& d : dirs) {
            try { d = fs::weakly_canonical(d); } catch (...) {}
        }
        std::sort(dirs.begin(), dirs.end());
        dirs.erase(std::unique(dirs.begin(), dirs.end()), dirs.end());

        std::vector<std::unique_ptr<calc::PluginLoader>> keep_alive;
        bool any_loaded = false;
        std::optional<std::string> last_err;

        for (const auto& d : dirs) {
            try {
                auto loader = std::make_unique<calc::PluginLoader>(d);
                loader->load(reg);
                keep_alive.push_back(std::move(loader));
                any_loaded = true;
                break;
            } catch (const std::exception& ex) {
                last_err = ex.what();
            }
        }
        if (!any_loaded && last_err) {
            std::cerr << "[Внимание] " << *last_err << '\n';
        }

        calc::Calculator calc(std::move(reg));
        std::cout << "Калькулятор (динамические плагины " << platform_ext() << ")\n";
        calc.print_functions(std::cout);
        std::cout << "Пример: 16 + 4 * (3 - 1) -> 24\n";
        std::cout << "Введите выражение. Пустая строка — выход.\n";

        std::string line;
        while (true) {
            std::cout << "> ";
            if (!std::getline(std::cin, line)) break;
            if (line.empty()) break;
            try {
                double r = calc.evaluate(line);
                std::cout << r << '\n';
            } catch (const std::exception& ex) {
                std::cout << "Ошибка: " << ex.what() << '\n';
            }
        }
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Критическая ошибка: " << ex.what() << '\n';
        return 1;
    }
}
