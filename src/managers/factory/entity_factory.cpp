


void InitEntityFactory(EntityFactory* entityFactory) {
	// Entity Manager

	entityFactory->entityCapacity = 100000;
	entityFactory->entities = (EntityInfo*)malloc(sizeof(EntityInfo) * entityFactory->entityCapacity);
	memset(entityFactory->entities, 0, sizeof(EntityInfo) * entityFactory->entityCapacity);
	entityFactory->nextID = 0;

       // InitEntityBuffers();
}
