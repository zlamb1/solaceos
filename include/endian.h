#ifndef ENDIAN_H
#define ENDIAN_H 1

#include <stdint.h>

typedef uint16_t __le16;
typedef uint16_t __be16;

typedef uint32_t __le32;
typedef uint32_t __be32;

typedef uint64_t __le64;
typedef uint64_t __be64;

uint16_t le16toh (__le16 n);
__le16 htole16 (uint16_t n);

uint16_t be16toh (__be16 n);
__be16 htobe16 (uint16_t n);

uint32_t le32toh (__le32 n);
__le32 htole32 (uint32_t n);

uint32_t be32toh (__be32 n);
__be32 htobe32 (uint32_t n);

uint64_t le64toh (__le64 n);
__le64 htole64 (uint64_t n);

uint64_t be64toh (__be64 n);
__be64 htobe64 (uint64_t n);

#endif