// LowLevelCode.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "heap_manager.h"

#include "raytracer/raytracer.h"

class MyObject
{
public:
    MyObject() :
        m_member(0)
    {
        std::cout << "MyObject Constructor\n";
    }

    ~MyObject()
    {
        std::cout << "MyObject Deconstructor\n";
    }

    void* operator new(size_t size)
    {
        std::cout << "Memory created using class new\n";
        return ::operator new(size);
    }

    void operator delete(void* memoryAddress)
    {
        std::cout << "Deleted using class delete\n";
        return ::operator delete(memoryAddress);
    }

private:
    int m_member;
};

int main()
{
    Raytracer* raytracer = new Raytracer();

    delete raytracer;
}