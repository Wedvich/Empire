#include "world.h"

void World::init() {
  // TODO: Load from data
  auto camera = createEntity();
}

void World::update(float deltaTime) {
  for (const auto& system : m_systems) {
    system->update(deltaTime);
  }
}

const Entity* World::createEntity() {
  auto entity = new Entity(m_nextEntityId++);
  m_entities.emplace(entity->id, std::unique_ptr<Entity>(entity));
  return entity;
}
