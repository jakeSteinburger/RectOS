#include <stdint.h>

#ifndef GDT_H
#define GDT_H

struct GDTEntry {
  uint16_t limit;
  uint16_t base;
  uint8_t base2;
  uint8_t access;
  uint8_t limit2 : 4;
  uint8_t flags : 4;
  uint8_t base3;
} __attribute__((packed));

struct GlobalDescriptor {
  uint16_t size;
  uint64_t offset;
} __attribute__((packed));

struct GDTEntry create_entry(uint16_t base, uint32_t limit, uint8_t access, uint8_t flags);

void create_table();

struct GlobalDescriptor create_descriptor();

__attribute__((noinline))
void load_gdt();

#endif
