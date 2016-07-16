

#ifndef ILA_H
#define ILA_H

#include <vnet/vnet.h>
#include <vnet/ip/ip.h>

#include <vppinfra/bihash_24_8.h>
#include <vppinfra/bihash_template.h>

typedef struct {
  u64 identifier;
  u64 locator;
  u64 sir_prefix;
  u32 adj_index;
  u32 ila_adj_index;
} ila_entry_t;

typedef struct {
  ila_entry_t *entries; //Pool of ILA entries

  u64 lookup_table_nbuckets;
  u64 lookup_table_size;
  BVT(clib_bihash) id_to_entry_table;
} ila_main_t;

int ila_add_entry(u64 identifier, u64 locator, u64 sir_prefix, u32 *entry_index);
int ila_interface(u32 sw_if_index, u8 disable);

#endif //ILA_H


