#ifndef VIGRIDR_UTILS_MAP_UTILS
#define VIGRIDR_UTILS_MAP_UTILS

namespace mjollnir { namespace vigridr { namespace utils {

template <typename M, typename V, typename K>
V getDefault(M map, K key, V defaultValue) {
  auto it = map.find(key);
  // TODO(luizmramos): CHECK
  if (it == map.end()) {
    return defaultValue;
  }
  return it->second;
}



}}}  // namespaces

#endif  // VIGRIDR_UTILS_MAP_UTILS
