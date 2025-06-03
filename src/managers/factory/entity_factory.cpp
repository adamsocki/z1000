
void InitEntityTypeInfo( Zayn* zaynMem) {
	constexpr EntityTypeInfoForBuffer defaultEntityTypeInfo[] = {
		// [EntityType_Player] = {EntityType_Player, sizeof(int32), 10, "Player"},
		// [EntityType_Floor]  = {EntityType_Floor, sizeof(int32), 10000, "Floor"},
		// [EntityType_Piano]  = {EntityType_Piano, sizeof(int32), 10, "Grand Piano"},
		[EntityType_Wall]  = {EntityType_Wall, sizeof(WallEntity), 100, "Wall"},
		// Add maor here...
	};

	memcpy(zaynMem->entityTypeInfoForBuffer, defaultEntityTypeInfo, sizeof(defaultEntityTypeInfo));
}


void InitEntityFactory(EntityFactory* entityFactory, Zayn* zaynMem) {
	// Entity Manager

	InitEntityTypeInfo(zaynMem);

	entityFactory->entityCapacity = 100000;
	entityFactory->entities = (EntityInfo*)malloc(sizeof(EntityInfo) * entityFactory->entityCapacity);
	memset(entityFactory->entities, 0, sizeof(EntityInfo) * entityFactory->entityCapacity);
	entityFactory->nextID = 0;

	// InitEntityBuffers(entityFactory, zaynMem);
}
EntityHandle AddEntity(EntityFactory* entityFactory, EntityType type) {
	int32 nextFreeIdInIndex = entityFactory->nextID;

	if (entityFactory->freeListCount > 0) {
		nextFreeIdInIndex = entityFactory->freeList[entityFactory->freeListCount - 1];
		entityFactory->freeListCount--;
	}
	else {
		entityFactory->nextID++;
	}

	EntityInfo* info = &entityFactory->entities[nextFreeIdInIndex];
	info->type = type;
	info->generation++;

	EntityTypeBuffer* buffer = &entityFactory->buffers[type];
	info->indexInBuffer = buffer->count;
	buffer->count++;

	EntityHandle handle = {};
	handle.generation = info->generation;
	handle.indexInInfo = nextFreeIdInIndex;
	handle.type = type;

	return handle;
}

// void MakeEntity()

// void CreateEntity(Zayn* zaynMem, EntityFactory* entityFactory) {
// 	EntityHandle newEntityHandle = {};
// 	AddEntity(entityFactory, &newEntityHandle, entityCreator.selectedEntityType);
//
// 	Entity* entity = static_cast<Entity*>(GetEntity(entityFactory, newEntityHandle));
//
//
// 	entity->name = "default";
//
// 	entity->material = &engine->materialFactory.materials[0];
//
//
//
// 	//
// 	// if (entityCreator.selectedEntityType == Game::EntityType::EntityType_Piano) {
// 	//     entity->mesh = &engine->meshFactory.meshes[1];
// 	//
// 	// }
// 	// else if (entityCreator.selectedEntityType == Game::EntityType::EntityType_Floor) {
// 	//     entity->mesh = &engine->meshFactory.meshes[0];
// 	//
// 	// }
// 	// else {
// 	//     entity->mesh = &engine->meshFactory.meshes[0];
// 	//
// 	// }
//
//
// 	//entity->type =
// 	//if (entity->entityType)
// 	// entity->type =
// 	// entity->pushConstantData.model_1 = TRS((V3(1.0f, 0.0f, 0.0f)), AxisAngle(V3(0.0f, 0.0f, 0.20f), 0.0f), V3(1.0f, 1.0f, 1.0f));
//
//
//
//
// 	// PushBack(&levelEditor->currentLevelData.levelEntityHandles, newEntityHandle);
//
//
// 	// levelEditor->currentLevelData
// 	// levelEditor->
//
// }
