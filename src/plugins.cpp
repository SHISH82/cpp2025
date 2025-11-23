#include "calc/plugins.hpp"
#include <iostream>
#include <cctype>
#include <utility>

#if defined(_WIN32)
    #define NOMINMAX
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

#include "calc/plugin_api.h"

namespace calc {


DynamicLibrary::DynamicLibrary(const fs::path& path) : path_(path) {
#if defined(_WIN32)
    handle_ = LoadLibraryW(path.wstring().c_str());
    if (!handle_) {
        throw std::runtime_error("Не удалось загрузить библиотеку: " + path.string());
    }
#else
    handle_ = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!handle_) {
        throw std::runtime_error(
            std::string("Не удалось загрузить библиотеку ") +
            path.string() + ": " + dlerror());
    }
#endif
}

DynamicLibrary::~DynamicLibrary() {
#if defined(_WIN32)
    if (handle_) {
        FreeLibrary((HMODULE)handle_);
    }
#else
    if (handle_) {
        dlclose(handle_);
    }
#endif
}

DynamicLibrary::DynamicLibrary(DynamicLibrary&& other) noexcept {
    swap(other);
}

DynamicLibrary& DynamicLibrary::operator=(DynamicLibrary&& other) noexcept {
    if (this != &other) {
        DynamicLibrary tmp(std::move(other));
        swap(tmp);
    }
    return *this;
}

void* DynamicLibrary::symbol(const char* name) const {
#if defined(_WIN32)
    auto s = GetProcAddress((HMODULE)handle_, name);
    if (!s) {
        throw std::runtime_error("Не найден символ: " + std::string(name));
    }
    return reinterpret_cast<void*>(s);
#else
    dlerror();
    void* s = dlsym(handle_, name);
    if (const char* e = dlerror()) {
        throw std::runtime_error(std::string("Ошибка поиска символа: ") + e);
    }
    if (!s) {
        throw std::runtime_error("Пустой символ: " + std::string(name));
    }
    return s;
#endif
}

void DynamicLibrary::swap(DynamicLibrary& other) noexcept {
    std::swap(path_,   other.path_);
    std::swap(handle_, other.handle_);
}


void FunctionRegistry::add_function(std::string name, UnaryFunction fn) {
    auto n = normalize(std::move(name));
    if (functions_.count(n)) {
        throw std::runtime_error("Дублирование функции: " + n);
    }
    functions_.emplace(std::move(n), std::move(fn));
}

bool FunctionRegistry::contains(const std::string& name) const {
    return functions_.count(normalize(name)) != 0;
}

FunctionRegistry::UnaryFunction
FunctionRegistry::get_function(const std::string& name) const {
    auto it = functions_.find(normalize(name));
    if (it == functions_.end()) {
        throw std::runtime_error("Неизвестная функция: " + name);
    }
    return it->second;
}

bool FunctionRegistry::empty() const noexcept {
    return functions_.empty();
}

std::vector<std::string> FunctionRegistry::names() const {
    std::vector<std::string> v;
    v.reserve(functions_.size());
    for (auto& [k, _] : functions_) {
        v.push_back(k);
    }
    std::sort(v.begin(), v.end());
    return v;
}

std::string FunctionRegistry::normalize(std::string s) {
    std::transform(
        s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return char(std::tolower(c)); });
    return s;
}


PluginLoader::PluginLoader(fs::path dir) : dir_(std::move(dir)) {}

void PluginLoader::load(FunctionRegistry& registry) {
    if (!fs::exists(dir_)) {
        throw std::runtime_error("Нет директории с плагинами: " + dir_.string());
    }

    std::size_t total = 0, matched = 0, registered = 0;

    for (const auto& e : fs::directory_iterator(dir_)) {
        if (!e.is_regular_file()) {
            continue;
        }
        ++total;
        const auto& p = e.path();

        bool ok = false;
#if defined(_WIN32)
        ok = (p.extension() == ".dll");
#elif defined(__APPLE__)
        ok = (p.extension() == ".dylib");
#else
        ok = (p.extension() == ".so");
#endif
        if (!ok) {
            continue;
        }
        ++matched;

        try {
            auto lib = std::make_unique<DynamicLibrary>(p);
            auto* regfn = reinterpret_cast<calc::plugin::RegisterFunction>(
                lib->symbol(calc::plugin::kRegistrationSymbol));
            if (!regfn) {
            }

            calc::plugin::FunctionDescriptor d{};
            if (!regfn(d)) {
                throw std::runtime_error("Плагин вернул неуспех");
            }
            if (d.abi_version != calc::plugin::kAbiVersion) {
                throw std::runtime_error("Несовместимая версия ABI");
            }
            if (!d.name || !d.invoke) {
                throw std::runtime_error("Плагин вернул пустой дескриптор");
            }

            if (registry.contains(d.name)) {
                continue;
            }

            registry.add_function(
                d.name,
                [fn = d.invoke, id = std::string(d.name)](double x) {
                    try {
                        return fn(x);
                    } catch (const std::exception& ex) {
                        throw std::runtime_error(
                            "Ошибка функции '" + id + "': " + ex.what());
                    } catch (...) {
                        throw std::runtime_error(
                            "Неизвестная ошибка функции '" + id + "'");
                    }
                });

            libs_.push_back(std::move(lib));
            ++registered;
        } catch (...) {
        }
    }

#if defined(_WIN32)
    const char* need = ".dll";
#elif defined(__APPLE__)
    const char* need = ".dylib";
#else
    const char* need = ".so";
#endif

    if (total == 0) {
        throw std::runtime_error(
            std::string("Каталог пуст. Ожидались плагины с расширением ") + need);
    }
    if (matched == 0) {
        throw std::runtime_error(
            std::string("Нет файлов плагинов с расширением ") + need);
    }
    if (registered == 0) {
        throw std::runtime_error(
            "Файлы плагинов найдены, но ни одна функция не зарегистрирована");
    }
}

}
