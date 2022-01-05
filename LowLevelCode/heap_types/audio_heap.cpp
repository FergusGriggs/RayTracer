#include "audio_heap.h"

AudioHeap::AudioHeap() :
    ManagedHeap(Type::eAudio)
{
}

AudioHeap::~AudioHeap()
{
}
