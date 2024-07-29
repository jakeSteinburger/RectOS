#include "gdt.h"
#include "../serial.h"
#include <stdint.h>

// some utilities that should be moved to another file later
void reverse(char str[], int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void uint64_to_hex_string(uint64_t num, char *str) {
    // Define a buffer large enough to hold the maximum uint64_t value in hex
    char buffer[17];  // Max length of uint64_t in hex is 16 digits, plus null terminator
    // Index to fill the buffer
    int index = 0;

    // Special case for zero
    if (num == 0) {
        buffer[index++] = '0';
    } else {
        // Extract hex digits from least significant to most significant
        while (num > 0) {
            uint8_t digit = num & 0xF;  // Get the least significant 4 bits (a hex digit)
            if (digit < 10) {
                buffer[index++] = '0' + digit;  // Convert digit to character
            } else {
                buffer[index++] = 'A' + (digit - 10);  // Convert digit to character
            }
            num >>= 4;  // Shift right by 4 bits to get the next hex digit
        }
    }
    // Null-terminate the buffer
    buffer[index] = '\0';
    // Reverse the buffer
    reverse(buffer, index);
    // Copy the reversed buffer to the output string (str)
    for (int i = 0; i <= index; ++i) {
        str[i] = buffer[i];
    }
}

struct GlobalDescriptor gdtr;

struct GDTEntry table[5];

uint64_t GDTRaw[5];

struct GDTEntry create_entry(uint16_t base, uint32_t limit, uint8_t access, uint8_t flags) {
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

void checkGDT() {
  char buffer[10];
  uint64_t tableSingle;
  for (int i = 0; i < 5; i++) {
    for (int y = 0; y < 10; y++)
      buffer[y] = 0;
    tableSingle = *(uint64_t*)&(table)[i];
    uint64_to_hex_string((uint64_t)tableSingle, buffer);
    write_serial("\nGDT contents: 0x");
    write_serial(buffer);
  }
}


void create_table() {
  struct GDTEntry null = create_entry(0, 0, 0, 0);
  struct GDTEntry kmcs = create_entry(0, 0xFFFFF, 0x9A, 0xA);
  struct GDTEntry kmds = create_entry(0, 0xFFFFF, 0x92, 0xC);
  struct GDTEntry umcs = create_entry(0, 0xFFFFF, 0xFA, 0xA);
  struct GDTEntry umds = create_entry(0, 0xFFFFF, 0xF2, 0xC);

  table[0] = null;
  table[1] = kmcs;
  table[2] = kmds;
  table[3] = umcs;
  table[4] = umds;
  checkGDT();
}

struct GlobalDescriptor create_descriptor() {
  struct GlobalDescriptor descriptor;
  uint16_t size = (sizeof(struct GDTEntry) * 5) - 1;

  descriptor.size = size;
  descriptor.offset = (uint64_t)&table;

  return descriptor;
}

__attribute__((noinline))
void load_gdt() {
  create_table();
  gdtr = create_descriptor();
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
              mov %%ax, %%ss" : : : "eax", "rax");
  write_serial("\nSuccess setting up the GDT.");
}







