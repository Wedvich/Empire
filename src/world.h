#pragma once

#include "systems/system.h"
#include "entities/entity.h"

class Renderer;

class World {
public:
  void init();
  void update(float deltaTime);
  const Entity* createEntity();

private:
  EntityId m_nextEntityId = 0;

  std::vector<std::unique_ptr<System>> m_systems;
  std::map<EntityId, std::unique_ptr<Entity>> m_entities;
};
