#include "crc_ec.hpp"
#include "protocol.h"
#include <algorithm>
#include <vector>

static std::vector<std::pair<uint32_t, size_t>> init_ec_table(const size_t len) {
    uint8_t *message = new uint8_t[len + 8]{};
    const uint32_t crc0 = crc32(message, len);
    std::vector<std::pair<uint32_t, size_t>> res;
    for (size_t i = 0; i < len; ++i)
        for (size_t j = 0; j < 8; ++j) {
            message[i] ^= (1 << j);
            const uint32_t crc = crc32(message, len);
            const size_t pos = (i << 3) | j;
            res.emplace_back(crc ^ crc0, pos);
            message[i] ^= (1 << j);
        }
    delete[] message;
    sort(res.begin(), res.end());
    return res;
}

static std::vector<std::pair<size_t, std::vector<std::pair<uint32_t, size_t>>>> ec_table_list;

static size_t query_table(const std::vector<std::pair<uint32_t, size_t>> &table, const uint32_t crc) {
    const auto it = std::lower_bound(table.begin(), table.end(), std::pair<uint32_t, size_t>(crc, 0));
    return it->first == crc ? it->second : SIZE_MAX;
}

static size_t query(const size_t len, const uint32_t crc) {
    for (const auto &[__l, __table] : ec_table_list)
        if (__l == len)
            return query_table(__table, crc);
    ec_table_list.emplace_back(len, init_ec_table(len));
    return query_table(ec_table_list.back().second, crc);
}

int crc_ec(uint8_t *message, const size_t len, const uint32_t crc) {
#if ECC
    size_t pos = query(len, crc);
    if (pos != SIZE_MAX) {
        message[pos >> 3] ^= (1 << (pos & 7));
        return 0;
    }
#endif

    return 1;
}
