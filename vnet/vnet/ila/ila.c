#include <vnet/ila/ila.h>

static ila_main_t ila_main;

#define ILA_TABLE_DEFAULT_HASH_NUM_BUCKETS (64 * 1024)
#define ILA_TABLE_DEFAULT_HASH_MEMORY_SIZE (32<<20)

#define foreach_ila_error                               \
 _(NONE, "valid ILA packets")

typedef enum {
#define _(sym,str) ILA_ERROR_##sym,
   foreach_ila_error
#undef _
   ILA_N_ERROR,
 } ila_error_t;

 static char *ila_error_strings[] = {
 #define _(sym,string) string,
   foreach_ila_error
 #undef _
 };

typedef enum {
  ILA_IN_NEXT_IP6_REWRITE,
  ILA_IN_NEXT_DROP,
  ILA_IN_N_NEXT,
} ila_in_next_t;

u8 *
format_ila_in_trace (u8 *s, va_list *args)
{
  return format(s, "ila-in");
}

static vlib_node_registration_t ila_in_node;

static uword
ila_in (vlib_main_t *vm,
        vlib_node_runtime_t *node,
        vlib_frame_t *frame)
{
  u32 n_left_from, *from, next_index, *to_next, n_left_to_next;
  vlib_node_runtime_t *error_node = vlib_node_get_runtime(vm, ila_in_node.index);
  __attribute__((unused)) ila_main_t *ilm = &ila_main;

  from = vlib_frame_vector_args(frame);
  n_left_from = frame->n_vectors;
  next_index = node->cached_next_index;

  while (n_left_from > 0) {
    vlib_get_next_frame(vm, node, next_index, to_next, n_left_to_next);

    /* Single loop */
    while (n_left_from > 0 && n_left_to_next > 0) {
      u32 pi0;
      vlib_buffer_t *p0;
      u8 error0 = ILA_ERROR_NONE;
      __attribute__((unused)) ip6_header_t *ip60;
      u32 next0 = ILA_IN_NEXT_DROP;

      pi0 = to_next[0] = from[0];
      from += 1;
      n_left_from -= 1;
      to_next +=1;
      n_left_to_next -= 1;

      p0 = vlib_get_buffer(vm, pi0);
      ip60 = vlib_buffer_get_current(p0);

      if (PREDICT_FALSE(p0->flags & VLIB_BUFFER_IS_TRACED)) {

      }

      p0->error = error_node->errors[error0];
      vlib_validate_buffer_enqueue_x1(vm, node, next_index, to_next, n_left_to_next, pi0, next0);
    }
    vlib_put_next_frame(vm, node, next_index, n_left_to_next);
  }

  return frame->n_vectors;
}

VLIB_REGISTER_NODE (ila_in_node, static) = {
  .function = ila_in,
  .name = "ila-in",
  .vector_size = sizeof (u32),

  .format_trace = format_ila_in_trace,

  .n_errors = ILA_N_ERROR,
  .error_strings = ila_error_strings,

  .n_next_nodes = ILA_IN_N_NEXT,
  .next_nodes = {
      [ILA_IN_NEXT_IP6_REWRITE] = "ip6-rewrite",
      [ILA_IN_NEXT_DROP] = "error-drop"
    },
};

typedef enum {
  ILA_OUT_NEXT_IP6_LOOKUP,
  ILA_OUT_NEXT_DROP,
  ILA_OUT_N_NEXT,
} ila_out_next_t;

u8 *
format_ila_out_trace (u8 *s, va_list *args)
{
  return format(s, "ila-out");
}

static vlib_node_registration_t ila_out_node;

static uword
ila_out (vlib_main_t *vm,
        vlib_node_runtime_t *node,
        vlib_frame_t *frame)
{
  u32 n_left_from, *from, next_index, *to_next, n_left_to_next;
  vlib_node_runtime_t *error_node = vlib_node_get_runtime(vm, ila_out_node.index);
  __attribute__((unused)) ila_main_t *ilm = &ila_main;

  from = vlib_frame_vector_args(frame);
  n_left_from = frame->n_vectors;
  next_index = node->cached_next_index;

  while (n_left_from > 0) {
    vlib_get_next_frame(vm, node, next_index, to_next, n_left_to_next);

    /* Single loop */
    while (n_left_from > 0 && n_left_to_next > 0) {
      u32 pi0;
      vlib_buffer_t *p0;
      u8 error0 = ILA_ERROR_NONE;
      __attribute__((unused)) ip6_header_t *ip60;
      u32 next0 = ILA_OUT_NEXT_DROP;

      pi0 = to_next[0] = from[0];
      from += 1;
      n_left_from -= 1;
      to_next +=1;
      n_left_to_next -= 1;

      p0 = vlib_get_buffer(vm, pi0);
      ip60 = vlib_buffer_get_current(p0);

      if (PREDICT_FALSE(p0->flags & VLIB_BUFFER_IS_TRACED)) {

      }

      p0->error = error_node->errors[error0];
      vlib_validate_buffer_enqueue_x1(vm, node, next_index, to_next, n_left_to_next, pi0, next0);
    }
    vlib_put_next_frame(vm, node, next_index, n_left_to_next);
  }

  return frame->n_vectors;
}

VLIB_REGISTER_NODE (ila_out_node, static) = {
  .function = ila_out,
  .name = "ila-out",
  .vector_size = sizeof (u32),

  .format_trace = format_ila_out_trace,

  .n_errors = ILA_N_ERROR,
  .error_strings = ila_error_strings,

  .n_next_nodes = ILA_OUT_N_NEXT,
  .next_nodes = {
      [ILA_OUT_NEXT_IP6_LOOKUP] = "ip6-lookup",
      [ILA_OUT_NEXT_DROP] = "error-drop"
    },
};

int ila_add_entry(u64 identifier, u64 locator,
                  u64 sir_prefix, u32 *entry_index)
{
  return 0;
}

int ila_interface(u32 sw_if_index, u8 disable)
{
  return 0;
}

clib_error_t *ila_init (vlib_main_t *vm) {
  ila_main_t *ilm = &ila_main;
  ilm->entries = NULL;

  ilm->lookup_table_nbuckets = ILA_TABLE_DEFAULT_HASH_NUM_BUCKETS;
  ilm->lookup_table_nbuckets = 1<< max_log2 (ilm->lookup_table_nbuckets);
  ilm->lookup_table_size = ILA_TABLE_DEFAULT_HASH_MEMORY_SIZE;

  BV(clib_bihash_init) (&ilm->id_to_entry_table, "ila id to entry index table",
                          ilm->lookup_table_nbuckets,
                          ilm->lookup_table_size);

  return NULL;
}

VLIB_INIT_FUNCTION(ila_init);

static clib_error_t *
ila_entry_command_fn (vlib_main_t *vm,
                          unformat_input_t *input,
                          vlib_cli_command_t *cmd)
{
  return NULL;
}

VLIB_CLI_COMMAND(ila_entry_command, static) = {
  .path = "ila entry",
  .short_help = "ila entry <identifier> <locator> <sir-prefix> [adj-index <adj-index>] [del] ",
  .function = ila_entry_command_fn,
};

static clib_error_t *
ila_interface_command_fn (vlib_main_t *vm,
                          unformat_input_t *input,
                          vlib_cli_command_t *cmd)
{
  return NULL;
}

VLIB_CLI_COMMAND(ila_interface_command, static) = {
  .path = "ila interface",
  .short_help = "ila interface <interface-name> [disable]",
  .function = ila_interface_command_fn,
};


