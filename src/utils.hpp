#pragma once

#include "logger.hpp"
#include <expected>
#include <memory>
#include <vector>
#include <cstring>

namespace ivmg::types {
    
template <typename T, typename E>
using ResultOr = std::expected<T, E>;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
    
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

template <typename T>
using Vec = std::vector<T>;

template <typename T>
using Unique = std::unique_ptr<T>;

template <typename T>
using Shared = std::shared_ptr<T>;

}

using namespace ivmg::types;


/**
 *   @brief Function to read any fundamental and trivially copyable
 *   type out of a read only byte buffer of any length. 
 *   Truncates if T is bigger than the remaining buffer size.
 *
 *   @tparam T The type to read out
 *   @tparam ordering The byte order of the data in the buffer
 *   @param data Non owning read only buffer of bytes
 *   @param idx Index from which to read the data. Reference for side effect for incrementing
 *   @returns Read data
 */
template <typename T, std::endian ordering = std::endian::little> 
requires std::is_trivially_copyable_v<T>
T read(std::span<const u8> data, size_t& idx) {
    T dest;

    size_t read_size_attempt = sizeof(T);
    size_t read_size_actual = read_size_attempt;

    if (idx + read_size_attempt > data.size()) 
        read_size_actual = read_size_attempt - ((idx + read_size_attempt) - data.size());

    std::memcpy(&dest, data.data()+idx, read_size_actual);
    idx += read_size_actual;

    LOG_LEVEL lvl = (read_size_attempt == read_size_actual) ? LOG_LEVEL::DEBG : LOG_LEVEL::WARNING;
    Logger::log(lvl, "Attempted to read {} bytes @ {:#x}, did read {}", 
        read_size_attempt, reinterpret_cast<std::uintptr_t>(data.data()+idx), read_size_actual);
    
    if (std::endian::native != ordering) return std::byteswap(dest);
    return dest;
}