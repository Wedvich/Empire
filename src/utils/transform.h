#pragma once

#include "components/transform.h"
#include "renderer/transform_proxy.h"

namespace Utils {
  void copyTransform(const TransformComponent& source, TransformProxy* target);
}
