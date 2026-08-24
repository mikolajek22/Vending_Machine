#include <core/uuid.hpp>

#include <random>
#include <array>
#include <sstream>
#include <iomanip>

namespace vending {

std::string uuid::generate_v4()
{
    std::array<uint8_t, 16> bytes{};
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<uint8_t> dist;

    for (auto &byte : bytes)
        byte = dist(generator);

    bytes[6] = (bytes[6] & 0xF) | 0x40;                 // set version of UUID to 4
    bytes[8] = bytes[8] & 0b00111111 | 0b10000000;      // 8/9/a/b

    std::ostringstream uuid;
    uuid << std::hex << std::setfill('0');
    for (std::size_t i = 0; i < bytes.size(); ++i)
    {
        uuid << std::setw(2) << static_cast<int>(bytes[i]);
        if (i == 3 || i == 5 || i == 7 || i == 9)
            uuid << '-';
    }
    return uuid.str();
}


} /* namespace vending */