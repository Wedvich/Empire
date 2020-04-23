#pragma once

#include "components\component.h"

typedef long EntityId;

class Entity {
public:
  const EntityId id;

private:
  friend class World;

  Entity(EntityId id) : id{id} {}
};
