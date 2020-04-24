#include "transform.h"

void Utils::copyTransform(const TransformComponent& source, TransformProxy* target) {
  target->scale = {source.scale};
  XMStoreFloat4(&target->rotate,
                XMQuaternionRotationRollPitchYaw(XMConvertToRadians(source.rotation.x),
                                                 XMConvertToRadians(source.rotation.y),
                                                 XMConvertToRadians(source.rotation.z)));
  target->translate = {source.position};
}
