
#pragma once

#include <vector>
#include <cstdint>
#include <span>

namespace File {
/**
 * @brief Alias for an 8-bit unsigned integer.
 */
using Byte = std::uint8_t;
/**
 * @brief Alias for a sequence of bytes, represented as a vector of bytes.
 */
using ByteSequence = std::vector<std::uint8_t>;
/*
 * @brief Alias for a span of constant bytes, used for read-only access to byte sequences. 
 */
using ByteSpan = std::span<const Byte>;
};
