#pragma once
#include <memory>
#include <cstdint>

#define _INTERNAL_CONCATENATE(LEFT, RIGHT) LEFT##RIGHT
#define CONCATENATE(LEFT, RIGHT) _INTERNAL_CONCATENATE(LEFT, RIGHT)

#define PAD(SIZE) char CONCATENATE(pad, __COUNTER__)[SIZE];

class Memory
{
public:
    Memory();

private:
    constexpr std::uint32_t CharToHexInt(const std::uint8_t uChar);
    std::size_t pattern_to_bytes(const char* pattern, std::uint8_t* byte_buf, char* mask_buf);

    std::uint8_t* find_pattern_ex(const std::uint8_t* region_start, const std::size_t region_size, const std::uint8_t* bytes, const std::size_t byte_count, const char* mask);
    std::uint8_t* find_pattern(const char* pattern, const char* mask);

public:
    std::uint8_t* find_pattern(const char* pattern);

public:
    __forceinline std::uint8_t* resolve_rel_addr(std::uint8_t* addr, std::uint32_t rva_offset, std::uint32_t rip_offset)
    {
        std::uint32_t rva = *reinterpret_cast<std::uint32_t*>(addr + rva_offset);
        std::uint64_t rip = reinterpret_cast<std::uint64_t>(addr) + rip_offset;

        return reinterpret_cast<std::uint8_t*>(rva + rip);
    }

    __forceinline void* get_base_address()
    {
        return base;
    }

    template <typename T, std::size_t idx, class base_class, typename... args>
    static __forceinline T call_vfunc(base_class* thisptr, args... arguments)
    {
        using virtual_fn = T(__thiscall*)(const void*, decltype(arguments)...);
        return (*reinterpret_cast<virtual_fn* const*>(reinterpret_cast<std::uintptr_t>(thisptr)))[idx](thisptr, arguments...);
    }

private:
    void* base;
};

inline std::unique_ptr<Memory> mem = std::make_unique<Memory>();