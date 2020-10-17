/* File author is Ítalo Lima Marconato Matias
 *
 * Created on June 20 of 2020, at 15:53 BRT
 * Last edited on August 24 of 2020, at 10:51 BRT */

#ifndef __SIA_HXX__
#define __SIA_HXX__

#include <cstdint>
#include <string>

using namespace std;

#define SIA_MAGIC 0xC4051AF0
#define SIA_INFO_KERNEL 0x01
#define SIA_INFO_FIXED 0x02
#define SIA_FLAGS_DIRECTORY 0x01
#define SIA_FLAGS_READ 0x02
#define SIA_FLAGS_WRITE 0x04
#define SIA_FLAGS_EXEC 0x08

struct __attribute__((packed)) sia_header_t {
	uint32_t magic;
	uint16_t info;
	uint8_t uuid[16];
	uint64_t free_file_count;
	uint64_t free_file_offset;
	uint64_t free_data_count;
	uint64_t free_data_offset;
	uint64_t kernel_offset;
	uint64_t root_offset;
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
	uint8_t contents[504];
};

bool sia_create(string path, string kernel, string base);

#endif
