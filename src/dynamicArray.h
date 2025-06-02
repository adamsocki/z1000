//
// Created by Adam Socki on 5/30/25.
//

struct ArrayChunk
    {
        ArrayChunk* nextChunk;
    alignas(8) uint8 data[];
    };

    struct DynamicArray_Untyped
    {
        MAllocator* allocator;

        uint32 count;
        uint32 elementsPerChunk;

        uint32 chunkCount;

        ArrayChunk* headChunk;
        ArrayChunk* tailChunk;
    };

template <typename T>
struct DynamicArray
{
    MAllocator* allocator;
    uint32 elementsPerChunk;
    uint32 count;
    uint32 chunkCount;
    ArrayChunk* headChunk;
    ArrayChunk* tailChunk;

    inline T& operator[](const int index) const
    {
        // Bounds check
        assert(index >= 0 && index < count);

        // Find the chunk
        s32 chunkIndex = index / elementsPerChunk;
        ArrayChunk* chunk = headChunk;

        for (int32 i = 0; i < chunkIndex && chunk != nullptr; i++)
        {
            chunk = chunk->nextChunk;
        }

        assert(chunk != nullptr);

        // Calculate offset within chunk
        size_t elementOffset = index % elementsPerChunk;

        // Get pointer to data area (immediately after chunk header)
        uint8* dataStart = (uint8*)chunk + sizeof(ArrayChunk);

        // Return reference to the element
        return ((T*)dataStart)[elementOffset];
    }
};


    template <typename T>
    void DynamicArrayAllocateChunk(DynamicArray<T>* array)
    {
        size_t headerSize = sizeof(ArrayChunk);
        size_t dataSize = array->elementsPerChunk * sizeof(T);
        size_t totalSize = headerSize + dataSize;

        // Allocate the entire block
        uint8* memory = (uint8*)AllocateMem(array->allocator, totalSize);

        // Set up the chunk header
        ArrayChunk* newChunk = (ArrayChunk*)memory;
        newChunk->nextChunk = nullptr;

        // Link the chunk
        if (array->headChunk == nullptr)
        {
            array->headChunk = newChunk;
            array->tailChunk = newChunk;
        }
        else
        {
            array->tailChunk->nextChunk = newChunk;
            array->tailChunk = newChunk;
        }

        array->chunkCount++;
    }

    inline void* DynamicArrayGetData(DynamicArray_Untyped const* array, int32 elementSize, int32 index) {

        s32 dynamicIndex = index / array->elementsPerChunk;
        ArrayChunk* chunk = array->headChunk;
        for (int32 i = 0; i < dynamicIndex; i++) {
            chunk = chunk->nextChunk;
        }

        return ((uint8*)chunk->data + (elementSize * (index % array->elementsPerChunk)));
    }

    template <typename T>
    void DynamicArrayEnsureCapacity(DynamicArray<T>* array, uint32 capacity)
    {
        // ASSERT(array->allocator != NULL);
        //ASSERT(array->elementsPerChunk > 0);

        if (array->chunkCount * array->elementsPerChunk < capacity)
        {
            uint32 chunksToAdd = ((capacity / array->elementsPerChunk) - array->chunkCount) + 1;

            for (int i = 0; i < chunksToAdd; i++)
            {
                DynamicArrayAllocateChunk(array);
            }
        }
    }

    void DynamicArrayAllocateDynamic(DynamicArray_Untyped* array, uint32 elementSize);
    void DynamicArrayEnsureCapacity(DynamicArray_Untyped* array, uint32 elementSize, uint32 capacity);

    template <typename T>
    inline DynamicArray<T> MakeDynamicArray(MAllocator* allocator, uint32 elementsPerChunk, uint32 chunkCount = 1)
    {
        DynamicArray<T> array = {};
        array.allocator = allocator;
        array.elementsPerChunk = elementsPerChunk;

        DynamicArrayEnsureCapacity(&array, chunkCount * elementsPerChunk);

        return array;
    };

    template <typename T>
    inline void DeallocateDynamicArray(DynamicArray<T>* array) {
        ArrayChunk* chunk = array->headChunk;
        ArrayChunk* nextChunk = chunk->nextChunk;
        while (chunk != NULL) {
            DeallocateMem(array->allocator, chunk);

            chunk = nextChunk;

            if (chunk) {
                nextChunk = nextChunk->nextChunk;
            }
        }
    }

    template <typename T>
    inline uint32 PushBack(DynamicArray<T>* array, T elem)
    {
        DynamicArrayEnsureCapacity(array, array->count + 1);
        uint32 index = array->count;
        array->count++;
        (*array)[index] = elem;
        return index;
    }

////template <typename T>
////inline T* PushBackPtr(DynamicArray<T>* array) {
////    DynamicArrayEnsureCapacity(array, array->count + 1);
////    uint32 index = array->count;
////    array->count++;
////    T* result = &(*array)[index];
////    memset(result, 0, sizeof(T));
////    return result;
////}
////
////template <typename T>
////inline bool PopBack(DynamicArray<T>* array, T* element = NULL)
////{
////    bool result = false;
////
////    if (array->count > 0) {
////        if (element != NULL) {
////            int32 index = array->count - 1;
////            *element = (*array)[index];
////        }
////        array->count--;
////        result = true;
////    }
////
////    return result;
////}

    template <typename T>
    inline void DynamicArrayClear(DynamicArray<T>* array)
    {
        array->count = 0;
    }


////inline void* PushBackPtr(DynamicArray_Untyped* array, u32 size)
////{
////    DynamicArrayEnsureCapacity(array, size, array->count + 1);
////    uint32 index = array->count;
////    array->count++;
////
////    void* ptr = DynamicArrayGetData(array, size, index);
////    memset(ptr, 0, size);
////    return ptr;
////}
////
////inline DynamicArray_Untyped MakeDynamicArray(MAllocator* allocator, uint32 size, uint32 elementsPerChunk, uint32 chunkCount = 1) {
////    DynamicArray_Untyped array = {};
////    array.allocator = allocator;
////
////    if (elementsPerChunk == 0) {
////        elementsPerChunk = 1;
////    }
////
////    array.elementsPerChunk = elementsPerChunk;
////
////    DynamicArrayEnsureCapacity(&array, size, elementsPerChunk);
////    return array;
////}