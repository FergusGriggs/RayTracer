// LowLevelCode.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "heap_manager.h"
#include "memory_pools/pool_manager.h"

#include "raytracer/raytracer.h"

#include "raytracer/objects/sphere.h"

class MyObject
{
public:
    MyObject() :
        m_member(0),
        m_member2ElectricBoogaloo("String that is not null")
    {
        std::cout << "MyObject Constructor\n";
    }

    ~MyObject()
    {
        std::cout << "MyObject Deconstructor\n";
    }

private:
    int m_member;
    std::string m_member2ElectricBoogaloo;
};

void test()
{
    HeapManager::it().printHeapUsages();

    std::cout << "Creating object list\n";
    Material testMat;
    std::vector<SphereSnapshot*> objectList;
    for (int i = 0; i < 1; i++)
    {
        objectList.push_back(new SphereSnapshot(Vec3f(0.0f), 1.0f, testMat, true));
    }

    HeapManager::it().printHeapUsages();

    std::cout << "Clearing object list\n";

    for (int i = 0; i < 1; i++)
    {
        delete objectList[i];
    }
    objectList.clear();
}

int main()
{
    PoolManager::it().initialise();
    HeapManager::it().initialiseSpecificHeaps();

    PoolManager::it().setDontUsePoolsOverride(false);

   // test();

    HeapManager::it().printHeapUsages();
    
    Raytracer* raytracer = new Raytracer();

    delete raytracer;

    PoolManager::it().clearPools();
}