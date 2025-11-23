#pragma once

#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

namespace calc {

    namespace fs = std::filesystem;

    class DynamicLibrary {
    public:
        explicit DynamicLibrary(const fs::path& path);
        ~DynamicLibrary();

        DynamicLibrary(const DynamicLibrary&) = delete;
        DynamicLibrary& operator=(const DynamicLibrary&) = delete;

        DynamicLibrary(DynamicLibrary&& other) noexcept;
        DynamicLibrary& operator=(DynamicLibrary&& other) noexcept;


        void* symbol(const char* name) const;

    private:
        fs::path path_;
        void*    handle_ = nullptr;

        void swap(DynamicLibrary& other) noexcept;
    };

    // Реестр функций-плагинов
    class FunctionRegistry {
    public:
        using UnaryFunction = std::function<double(double)>;

        void add_function(std::string name, UnaryFunction fn);
        bool contains(const std::string& name) const;
        UnaryFunction get_function(const std::string& name) const;
        bool empty() const noexcept;
        std::vector<std::string> names() const;

    private:
        std::map<std::string, UnaryFunction> functions_;

        static std::string normalize(std::string s);
    };

    class PluginLoader {
    public:
        explicit PluginLoader(fs::path dir);

        void load(FunctionRegistry& registry);

    private:
        fs::path dir_;
        std::vector<std::unique_ptr<DynamicLibrary>> libs_;
    };

}
