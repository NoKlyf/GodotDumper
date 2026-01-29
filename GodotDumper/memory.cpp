#include "memory.h"
#include <cctype>
#include <Windows.h>

Memory::Memory()
{
    base = (void*)GetModuleHandleA(nullptr);
}

constexpr std::uint32_t Memory::CharToHexInt(const std::uint8_t uChar)
{
    const std::uint8_t uCharLower = (uChar | ('a' ^ 'A'));
    return ((uCharLower >= 'a' && uCharLower <= 'f') ? (uCharLower - 'a' + 0xA) : (std::isdigit(uChar) ? (uChar - '0') : 0x0));
}

std::size_t Memory::pattern_to_bytes(const char* pattern, std::uint8_t* byte_buf, char* mask_buf)
{
    std::uint8_t* current_byte = byte_buf;

    while (*pattern != '\0')
    {
        if (*pattern == '?')
        {
            ++pattern;

            *current_byte++ = 0;
            *mask_buf++ = '?';
        }
        else if (*pattern != ' ')
        {
            std::uint8_t byte = static_cast<std::uint8_t>(CharToHexInt(*pattern) << 4);

            ++pattern;
            byte |= static_cast<std::uint8_t>(CharToHexInt(*pattern));

            *current_byte++ = byte;
            *mask_buf++ = 'x';
        }

        ++pattern;
    }

    *current_byte = 0;
    *mask_buf = '\0';

    return current_byte - byte_buf;
}

std::uint8_t* Memory::find_pattern_ex(const std::uint8_t* region_start, const std::size_t region_size, const std::uint8_t* bytes, const std::size_t byte_count, const char* mask)
{
    std::uint8_t* curr_addr = const_cast<std::uint8_t*>(region_start);
    const std::uint8_t* region_end = region_start + region_size - byte_count;
    const bool mask_used = mask != nullptr;

    while (curr_addr < region_end)
    {
        if ((mask_used && *mask == '?') || *curr_addr == *bytes)
        {
            if (byte_count == 1)
                return curr_addr;

            std::size_t compared_bytes = 0;
            while ((mask_used && mask[compared_bytes + 1] == '?') || curr_addr[compared_bytes + 1] == bytes[compared_bytes + 1])
            {
                if (++compared_bytes == byte_count - 1)
                    return curr_addr;
            }

            curr_addr += compared_bytes;
        }

        ++curr_addr;
    }

    return nullptr;
}

std::uint8_t* Memory::find_pattern(const char* pattern, const char* mask)
{
    const std::uint8_t* base_addr = reinterpret_cast<const std::uint8_t*>(base);

    const IMAGE_DOS_HEADER* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE)
        return nullptr;

    const IMAGE_NT_HEADERS* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base_addr + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE)
        return nullptr;

    const std::uint8_t* bytes = reinterpret_cast<const std::uint8_t*>(pattern);
    const std::size_t byte_count = strlen(mask);

    std::uint8_t* found_addr = find_pattern_ex(base_addr, nt->OptionalHeader.SizeOfImage, bytes, byte_count, mask);
    return found_addr;
}

std::uint8_t* Memory::find_pattern(const char* pattern)
{
    const std::size_t approx_buff_size = (strlen(pattern) >> 1) + 1;
    std::uint8_t* bytes = static_cast<std::uint8_t*>(_malloca(approx_buff_size));
    char* mask = static_cast<char*>(_malloca(approx_buff_size));
    pattern_to_bytes(pattern, bytes, mask);

    return find_pattern(reinterpret_cast<const char*>(bytes), mask);
}