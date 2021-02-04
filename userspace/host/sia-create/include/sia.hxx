/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 29 of 2021, at 10:20 BRT
 * Last edited on January 29 of 2021, at 12:00 BRT */

#pragma once

#include <cstdint>
#include <fstream>
#include <string>

#define SIA_MAGIC 0xC4051AF0

#define SIA_FIXED 0x01

/* Arch-specific file flags for the kernel. */

#define SIA_X86 0x01

#define SIA_ARM64 0x02
#define SIA_ARM64_39_BITS 0x08
#define SIA_ARM64_48_BITS 0x10

#define SIA_AMD64 0x04
#define SIA_AMD64_48_BITS 0x08
#define SIA_AMD64_57_BITS 0x10

/* And file flags for normal files/directories. */

#define SIA_DIR 0x01
#define SIA_READ 0x02
#define SIA_WRITE 0x04
#define SIA_EXEC 0x08

struct __attribute__((packed)) sia_header_t {
    uint32_t magic;
    uint16_t info;
    uint8_t id[16];
    uint64_t free_file_count;
    uint64_t free_file_offset;
    uint64_t free_data_count;
    uint64_t free_data_offset;
    uint64_t kernel_images[16];
    uint64_t root_images[16];
};

struct __attribute__((packed)) sia_file_t {
    uint64_t next;
    uint16_t flags;
    uint64_t size;
    uint64_t offset;
    char name[64];
};

struct __attribute__((packed)) sia_data_t {
    uint64_t next;
    uint8_t data[504];
};

struct sia_t {
    std::fstream &file;
    sia_header_t &header;
    int8_t last_kernel_image;
    int8_t last_root_image;
};

bool sia_add_image(sia_t&, std::string);
bool sia_add_kernel(sia_t&, std::string, uint16_t);
bool sia_init(sia_t&);
