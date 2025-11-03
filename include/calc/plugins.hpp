#pragma once
#include <functional>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <memory>
#include <stdexcept>

namespace fs = std::filesystem;

namespace calc {

    class FunctionRegistry {
    public:
        using UnaryFunction = std::function<double(double)>;
        void add_function(std::string name, UnaryFunction fn);
        bool contains(const std::string& name) const;
        UnaryFunction get_function(const std::string& name) const;
        bool empty() const noexcept;
        std::vector<std::string> names() const;

        static std::string normalize(std::string s);

    private:
        std::map<std::string, UnaryFunction> functions_;
    };

    class DynamicLibrary {
    public:
        explicit DynamicLibrary(const fs::path& path);
        ~DynamicLibrary();
        DynamicLibrary(DynamicLibrary&&) noexcept;
        DynamicLibrary& operator=(DynamicLibrary&&) noexcept;

        void* symbol(const char* name) const;

    private:
        void* handle_{};
        fs::path path_;
        void swap(DynamicLibrary& other) noexcept;
    };

    class PluginLoader {
    public:
        explicit PluginLoader(fs::path dir);
        void load(FunctionRegistry& registry);

    private:
        fs::path dir_;
        std::vector<std::unique_ptr<DynamicLibrary>> libs_;
    };

} // namespace calc
