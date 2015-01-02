#pragma once
#include <Macro.h>

#ifndef offsetof
//#	define offsetof(s,m)   (size_t)&((s *)0)->m
#define offsetof(TYPE, MEMBER) __builtin_offsetof (TYPE, MEMBER)
#endif

#include <cstdint>
#ifdef _WIN32
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	undef WIN32_LEAN_AND_MEAN
#elif defined(__LINUX__) || defined(__linux__)
extern "C"{
#	define __lint__
#	include <sys/time.h>
}
#endif

class Class;
class Object;

class Field{
private:
    Class* clazz = nullptr;
    Class* type = nullptr;
    std::string fieldName;
    int offset = 0;

public:
    Field(){}
    Field(Class* clazz, const std::string& fieldName, int offset, Class* type){
        this->clazz = clazz;
        this->fieldName = fieldName;
        this->offset = offset;
        this->type = type;
    }

    int GetOffset() const{
        return offset;
    }

    template<typename T>
    void Set(void* obj, T value){
        *(T*)((char*)obj + offset) = value;
    }

    template<typename T>
    T& GetT(void* obj) const{
        return *(T*)((char*)obj + offset);
    }

    Object* Get(void* obj);

    void Set(Object* obj, Object* value);

    Class* GetType(){
        return type;
    }
};

class Class{
public:
    typedef void* (*NEW_FUN)();
    typedef void* (*CAST_FUN)(void*);

private:

    std::string name;
    NEW_FUN _new = nullptr;
    Class* base = nullptr;
    int size = 0;

    std::unordered_map<std::string, Field*> fields;
    std::unordered_map<Class*, CAST_FUN> casts;

public:
    Class() = delete;
    Class(const Class&) = delete;
    Class(Class&&) = delete;
    Class& operator=(const Class&) = delete;
    Class& operator=(Class&&) = delete;

    static std::unordered_map<std::string, Class*>& GetClasses(){
        static std::unordered_map<std::string, Class*> classes;
        return classes;
    }

    ~Class(){
        for (auto p : fields){
            delete p.second;
        }
    }

    Class(const std::string& name, int size, NEW_FUN _new = nullptr){
        this->name = name;
        this->size = size;
        this->_new = _new;
        GetClasses().insert(std::pair<std::string, Class*>(name, this));
    }

    Class(const std::string& name, Class* base, int size, NEW_FUN _new, ...){
        this->name = name;
        this->base = base;
        this->size = size;
        this->_new = _new;
        GetClasses().insert(std::pair<std::string, Class*>(name, this));

        va_list arg;
        va_start(arg, _new);
        while (true){
            char *field = va_arg(arg, char*);
            if (field == nullptr)
                break;
            int offset = va_arg(arg, int);
            Class* type = va_arg(arg, Class*);
            fields.insert(std::pair<std::string, Field*>(field, new Field(this, field, offset, type)));
        }
    }

    Field* GetField(const std::string& fieldName){
        Field* field = fields[fieldName];
        auto c = base;
        while (field == nullptr && c != nullptr){
            field = c->fields[fieldName];
            c = c->base;
        }
        if (field == nullptr){
            //throw std::runtime_error("field '" + fieldName + "' not found");
        }

        return field;
    }

    const std::unordered_map<std::string, Field*>& GetFields() const{
        return fields;
    }

    const std::string& GetName() const{
        return name;
    }

    int GetSize() const{
        return size;
    }

    Class* GetBaseClass() const{
        return base;
    }

    bool IsDerivedFrom(Class* clazz){
        auto c = this;
        while (c){
            if (c == clazz)
                return true;
            c = c->base;
        }

        return false;
    }

    void AddCast(Class* type, CAST_FUN fun){
        casts.insert(std::pair<Class*, CAST_FUN>(type, fun));
    }

    bool CanCastFrom(Class* type){
        return casts[type];
    }

    void* CastFrom(Object* obj);

    void* CastFrom(int data);

    void* New() const{
        if (_new)
            return _new();
        else
            return nullptr;
    }

    static Class* ForName(const std::string& name){
        Class* clazz = GetClasses()[name];
        if (clazz == nullptr){
            //throw std::runtime_error("class '" + name + "' not found");
        }

        return clazz;
    }

};

#define DECL_CLASS(class_name) \
public: \
    static Class clazz; \
    virtual Class* GetClass() const{ \
        return &clazz; \
    } \
    static class_name* Create(){ \
        return new class_name(); \
    } \
private:
#define DECL_ABS_CLASS(class_name) \
public: \
    static Class clazz; \
    virtual Class* GetClass() const{ \
        return &clazz; \
    } \
private:
#define IMPL_CLASS(class_name, base) Class class_name::clazz(#class_name, &base::clazz, sizeof(class_name), (Class::NEW_FUN)class_name::Create, nullptr);
#define IMPL_CLASS_WITH_FIELDS(class_name, base, ...) Class class_name::clazz(#class_name, &base::clazz, sizeof(class_name), (Class::NEW_FUN)class_name::Create, __VA_ARGS__, nullptr);
#define IMPL_ABS_CLASS(class_name, base) Class class_name::clazz(#class_name, &base::clazz, sizeof(class_name), nullptr, nullptr);
#define IMPL_ABS_CLASS_WITH_FIELDS(class_name, base, ...) Class class_name::clazz(#class_name, &base::clazz, sizeof(class_name), nullptr, __VA_ARGS__, nullptr);
#define FIELD(class_name, field_name, field_type) #field_name, offsetof(class_name, field_name), &field_type::clazz
#define ALIAS_FIELD(alias_name, child_name, child_type, class_name, field_name, field_type) #alias_name, offsetof(class_name, field_name) + offsetof(child_type, child_name), &field_type::clazz


class BaseType{
private:
    virtual void abstract_func() = 0;
public:
    static Class clazz;
};

#define DECL_BASETYPE(old_name, new_name) \
class new_name : public BaseType{ \
public: \
    static Class clazz; \
};
#define IMPL_BASETYPE(old_name, new_name) \
Class new_name::clazz(#old_name, &BaseType::clazz, sizeof(old_name), nullptr, nullptr);

DECL_BASETYPE(int, Integer)
DECL_BASETYPE(float, Float)
DECL_BASETYPE(double, Double)
DECL_BASETYPE(long, Long)
DECL_BASETYPE(bool, Bool)
DECL_BASETYPE(string, String)

class Object{
public:
    DECL_CLASS(Object);

    virtual std::string ToString() const{
        std::stringstream ss;
        ss << GetClass()->GetName() << "[" << (void*)this << "]";
        return  ss.str();
    }

    bool Is(Class* clazz) const{
        Class* this_class = &this->clazz;
        while (clazz != nullptr){
            if (this_class == clazz)
                return true;
            clazz = clazz->GetBaseClass();
        }
        return GetClass() == clazz;
    }
};
