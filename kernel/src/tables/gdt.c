#include "gdt.h"
#include <stdint.h>

struct GDTEntry create_entry(uint16_t base, uint32_t limit, uint8_t access,
                             uint8_t flags) {

  struct GDTEntry entry;

  entry.base = 0;
  entry.base2 = 0;
  entry.base3 = 0;
  entry.limit = limit & 0xFFFF;
  entry.limit2 = (limit >> 16) & 0xF;
  entry.access = access;
  entry.flags = flags;

  return entry;
}

struct GlobalDescriptorTable create_table() {
  struct GlobalDescriptorTable table;

  struct GDTEntry null = create_entry(0, 0, 0, 0);
  struct GDTEntry kmcs = create_entry(0, 0xFFFFF, 0x9A, 0xA);
  struct GDTEntry kmds = create_entry(0, 0xFFFFF, 0x92, 0xC);

  table.null = null;
  table.kmcs = kmcs;
  table.kmds = kmds;

  return table;
}

struct GlobalDescriptor create_descriptor(struct GlobalDescriptorTable *table) {
  struct GlobalDescriptor descriptor;
  uint16_t size = (sizeof(struct GlobalDescriptorTable)) - 1;

  descriptor.size = size;
  descriptor.offset = (uint64_t)table;

  return descriptor;
}

void load_gdt() {

  struct GlobalDescriptorTable gdt = create_table();

  struct GlobalDescriptor gdtr = create_descriptor(&gdt);

  asm volatile("lgdt (%0)" : : "r"(&gdtr));

  asm volatile("push $0x08; \
                  lea .reload_CS(%%rip), %%rax; \
                  push %%rax; \
                  retfq; \
                  .reload_CS: \
                  mov $0x10, %%ax; \
                  mov %%ax, %%ds; \
                  mov %%ax, %%es; \
                  mov %%ax, %%fs; \
                  mov %%ax, %%gs; \
                  mov %%ax, %%ss"
               :
               :
               : "eax", "rax");
}
