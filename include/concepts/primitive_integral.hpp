#ifndef PRIMITIVE_INTEGRAL_HPP
#define PRIMITIVE_INTEGRAL_HPP
#include <concepts>
#include <type_traits>

/**
 * Some fundamental integer type (char, short, int, long, long long and unsigned variants).
 */
template <typename T>
concept PrimitiveIntegral = std::integral<T> && std::is_fundamental_v<T> && !std::same_as<T, bool>; 
//Booleans are considered integral under the hood (1 or 0), so they're excluded explicitly

#endif