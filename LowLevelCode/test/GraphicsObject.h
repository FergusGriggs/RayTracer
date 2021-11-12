#pragma once

class GraphicsObject
{
public:
    GraphicsObject();
    ~GraphicsObject();

    void* operator new(size_t size);
    void* operator new[](size_t size);

private:
    int m_bigArray[100];
};

