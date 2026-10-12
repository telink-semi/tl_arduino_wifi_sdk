#pragma once
/*
 * Auto-generated 512KB flash tail layout (tlsr_flash_parts[]).
 *
 * Validation scope: overlaps and bounds are checked only inside
 *   [TLSR_FLASH_TAIL_BASE, TLSR_FLASH_TAIL_BASE + 0x80000).
 * OTA slots, application images, and other regions outside this window are
 * not checked here — ensure the product linker / image map does not overlap
 * this tail window.
 */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct {
  const char *name;
  uint32_t start; /* inclusive absolute flash address */
  uint32_t end;   /* exclusive absolute flash address */
  uint8_t is_system;
  uint8_t allowed_perm; /* bitmask: R=1, W=2, E=4 */
} tlsr_flash_part_desc_t;

extern const tlsr_flash_part_desc_t tlsr_flash_parts[];
extern const size_t tlsr_flash_parts_count;

/* Compile-time constants for use in static initializers */
#define TLSR_FLASH_TAIL_BASE   ((uint32_t)0x80380000)
#define TLSR_FLASH_USER_START  ((uint32_t)0x80388000)
#define TLSR_FLASH_USER_END    ((uint32_t)0x803ff000)

/* Runtime constants (same values as the macros above) */
extern const uint32_t tlsr_flash_tail_base;
extern const uint32_t tlsr_flash_user_start;
extern const uint32_t tlsr_flash_user_end;

