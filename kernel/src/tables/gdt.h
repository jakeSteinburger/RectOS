#include <stdint.h>
struct GDTEntry {
  uint16_t limit;
  uint16_t base;
  uint8_t base2;
  uint8_t access;
  uint8_t limit2 : 4;
  uint8_t flags : 4;
  uint8_t base3;
};

struct GlobalDescriptorTable {
  struct GDTEntry null;
  struct GDTEntry kmcs;
  struct GDTEntry kmds;
};

struct GlobalDescriptor {
  uint16_t size;
  uint64_t offset;
};

struct GDTEntry create_entry(uint16_t base, uint32_t limit, uint8_t access,
                             uint8_t flags);

struct GlobalDescriptorTable create_table();

struct GlobalDescriptor create_descriptor(struct GlobalDescriptorTable *);

void load_gdt();
