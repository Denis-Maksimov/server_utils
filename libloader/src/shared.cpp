
#include <iostream>

#if defined(_WIN32)
    #include <windows.h>
#elif __linux__
    #include <dlfcn.h>
    #include <locale.h>

    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
    
#else 
    #error "op system is not supported"
#endif



#include <wchar.h>
#include <wctype.h>
#include <cstring>

#include "shared.hpp"
#include "pass.hpp"



bool common::check_name_encoding(const char *name) 
{
    #if defined(_WIN32)
        size_t len = strlen(name) + 1;
        wchar_t *wname = (wchar_t*)malloc(len * sizeof(wchar_t));
        MultiByteToWideChar(CP_ACP, 0, name, -1, wname, len);
        for (size_t i = 0; i < len - 1; i++) {
            if (!iswgraph(wname[i])) {
                free(wname);
                return false; // Имя файла содержит недопустимые символы
            }
        }
        free(wname);
        return true; // Имя файла корректно

    #else
        setlocale(LC_CTYPE, ""); // устанавливаем текущую локаль для текущей категории вывода (здесь категория — символы)
        size_t len = strlen(name) + 1;
        wchar_t *wname = (wchar_t*)malloc(len * sizeof(wchar_t));
        mbstowcs(wname, name, len);
        for (size_t i = 0; i < len - 1; i++) 
        {
            if (!iswgraph(wname[i])) {
                free(wname);
                return false; // Имя файла содержит недопустимые символы
            }
        }
        free(wname);
        return true; // Имя файла корректно
    
    #endif
}


bool common::is_c_str(const char *name)
{
    if (name == nullptr) {
        // Обработка ошибки: передан пустой указатель
        // std::cerr<<name<<"0 ptr\n";
        return false;
    }

    size_t len = strlen(name);
    if (len == 0 || name[len] != '\0') 
    {
        // Обработка ошибки: строка не содержит нуль-символа в конце
        // std::cerr<<"0 term\n";
        return false;
    }

    return true;
}


bool common::check_extension(const char *name,const char *ext) 
{
    if(!is_c_str(name)) return false;
    if(!is_c_str(ext)) return false;

    if(!check_name_encoding(name)) return false;
    if(!check_name_encoding(ext)) return false;

    // Получение расширения имени файла
    const char *extension = strrchr(name, '.');
    if (extension == NULL) {
        // Обработка ошибки отсутствия расширения в имени файла
        return false;
    }

    // Проверка расширения имени файла
    if (strcasecmp(extension, ext) == 0) 
    {
        return true;
    }
    
    // Обработка некорректного расширения имени файла
    return false;
}

bool common::check_libname(const char *name) 
{
    if(!is_c_str(name))           { return false;}
    if(!check_name_encoding(name))  {return false;}
    
    const char * ext = 
        #if defined(_WIN32)
            ".dll";
        #elif __linux__
            ".so";
        #else 
            #error "OS no supported!"
        #endif


    if (!check_extension(name,ext)) {  return false;}


    // Обработка строки: все проверки пройдены успешно
    return true;
}




// windows
#if defined(_WIN32)
class winLib: public BaseType 
{
    struct libDeleter
    {
        void
        operator()(HINSTANCE* instance)
        {
            FreeLibrary(instance);
        }
    };
    using libUniquePtr=std::unique_ptr<HINSTANCE, libDeleter>;

public:
    HINSTANCE instance = nullptr;
    winLib(const char* name)
    {
        if(!common::check_libname(name)) throw;
        instance=libUniquePtr(LoadLibrary(name));
    }

    void* get_function(const char* fname)
    {
        if (instance) 
        {
            return GetProcAddress(instance, fname);
        }
        return nullptr;
    }
};
#endif



// linux
#if defined(__linux__)
class tuxLib: public BaseType 
{
    struct libDeleter
    {
        void
        operator()(void* instance)
        {
            dlclose(instance);
        }
    };
    using libUniquePtr=std::unique_ptr<void, libDeleter>;
public:
    libUniquePtr instance=nullptr;
    tuxLib(const char* name)
    {
        if(!common::check_libname(name)) throw std::runtime_error("4");
        instance=libUniquePtr(dlopen(name, RTLD_LAZY));
    }

    void* get_function(const char* fname)
    {
        if (instance) 
        {
            return dlsym(instance.get(), "add");                
        }
        return nullptr;
    }
};
#endif

#ifndef 1 //TODO
// linux
#if defined(__linux__)
class tuxEncLib: public BaseType 
{
    struct libDeleter
    {
        void
        operator()(void* instance)
        {
            dlclose(instance);
        }
    };
    using libUniquePtr=std::unique_ptr<void, libDeleter>;
public:
    libUniquePtr instance=nullptr;
    tuxEncLib(const char* name)
    {
        if(!common::check_libname(name)) throw std::runtime_error("4");
        instance=libUniquePtr(dlopen(name, RTLD_LAZY));
    }

    void* get_function(const char* fname)
    {
        if (instance) 
        {
            return dlsym(instance.get(), "add");                
        }
        return nullptr;
    }

    int load_library_into_memory(const char *libpath,const char *passfile)
    {
        // Открываем файл библиотеки

        auto ss=decrypt_password_file_to_mem(libpath,passfile);
        size_t size = ss.str().size();


        void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ptr == MAP_FAILED) 
        {
            std::cerr << "Error: failed to mmap." << std::endl;
            return 1;
        }
        char* c_ptr = reinterpret_cast<char*>(ptr);
        ss.read(c_ptr, size);

        void* libptr = mremap(ptr, size, size + getpagesize(), MREMAP_MAYMOVE);
        if (libptr == MAP_FAILED) {
            std::cerr << "Error: failed to mremap." << std::endl;
            return 1;
        }
        return 0;
    }


};
#endif
#endif


// Представитель

library::library(const char* name) 
{
    // Определение архитектуры при создании экземпляра

    #if defined(_WIN32)
        concreteType = new winLib(name);
    #elif __linux__
        concreteType = new tuxLib(name);
    #else 
        #error "OS no supported!"
    #endif
}


void* library::get_function(const char* fname)
{
    return concreteType->get_function(fname);
}



typedef int (*add_t)(int ,int );

// int main() {

//    library lib("./mylib.so");
//    auto fu=(add_t)lib.get_function("add");
//     std::cout << fu(2,8)<<std::endl;
//     return 0;
// }

