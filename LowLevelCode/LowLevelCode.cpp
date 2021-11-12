// LowLevelCode.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "test/GraphicsObject.h"

#include "HeapManager.h"

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
    //MyObject* newObj = new MyObject();

    //delete newObj;
    GraphicsObject* obj1 = new GraphicsObject();
    GraphicsObject* obj2 = new GraphicsObject();
    GraphicsObject* obj3 = new GraphicsObject();

    delete obj1;
    delete obj2;
    delete obj3;
    //int number = 7;

    //int* numberArray = new int[10];
    //void* mallocNumberArray = malloc(sizeof(int));

    //std::cout << "The number is " << number;

    //free(mallocNumberArray);
    //delete[] numberArray;
}