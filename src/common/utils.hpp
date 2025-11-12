#pragma once

#include "logger.hpp"
#include <cstring>


constexpr uint8_t BYTE_PER_PIXEL = 4;  // 4 channels @ 8 bits



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
T read(std::span<const uint8_t> data, size_t& idx) {
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
