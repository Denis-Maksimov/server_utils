#ifndef SHARED
#define SHARED
#include <memory>

// Базовый класс
class BaseType 
{
public:
    virtual ~BaseType() {}
    virtual void* get_function(const char* fname)=0;
};




// Представитель
class library 
{
private:
    BaseType* concreteType;

public:
    library(const char* name);
    void* get_function(const char* fname);
};


namespace common
{
    bool check_name_encoding(const char *name);
    bool is_c_str(const char *name);
    bool check_extension(const char *name,const char *ext);
    bool check_libname(const char *name);
}



#endif /* SHARED */
