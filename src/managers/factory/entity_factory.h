

enum EntityType 
{
    EntityType_Player,
        EntityType_Floor,
        EntityType_Piano,
        EntityType_Wall,
        EntityType_Count
    };


struct EntityTypeInfoForBuffer
    {
        EntityType type;
        int32 structSize;
        int32 defaultCapacity;
        const char* typeName;
    };


struct FreeList {
        int32 freeList[1000];
        int32 freelistCount;
    };

struct EntityHandle
{
	int32 generation;
        int32 indexInInfo;
        EntityType type;

        bool operator<(const EntityHandle& other) const {
            if (indexInInfo != other.indexInInfo) return indexInInfo < other.indexInInfo;
            if (generation != other.generation) return generation < other.generation;
            return type < other.type;
        }

        // For std::unordered_map
        bool operator==(const EntityHandle& other) const {
            return indexInInfo == other.indexInInfo &&
                   generation == other.generation &&
                   type == other.type;
        }
};

struct EntityInfo {
        int32 generation;
        int32 indexInBuffer;

        EntityType type;
    };

    struct EntityTypeBuffer {
        int32 count;
        int32 capacity;
        int32 sizeInBytes;

        void* entities;
    };


struct EntityFactory
{
    EntityTypeBuffer buffers[EntityType_Count];
    EntityInfo *entities;
    int32 entityCapacity;

    int32 freeListCount;
    int32 freeList[10000];

//        FreeList levelIDtest[10];

    int32 nextID;

    DynamicArray<EntityHandle> activeEntityHandles;
};