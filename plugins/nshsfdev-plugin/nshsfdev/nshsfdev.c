

#include <nshsfdev/nshsfdev.h>

#include <vnet/plugin/plugin.h>
#include <dlfcn.h>

#include <nshsfdev/nshsfdev_api.h>

#define foreach_nshsfdev_error \
 _(NONE, "valid packet")

typedef enum {
#define _(sym,str) NSHSFDEV_ERROR_##sym,
  foreach_nshsfdev_error
#undef _
    NSHSFDEV_N_ERROR,
} nshsfdev_error_t;

static char *nshsfdev_error_strings[] = {
#define _(sym,string) string,
    foreach_nshsfdev_error
#undef _
};

typedef enum {
  NSHSFDEV_NEXT_DROP,
  NSHSFDEV_N_NEXT,
} nshsfdev_next_t;

clib_error_t *
vlib_plugin_register (vlib_main_t * vm, vnet_plugin_handoff_t * h,
		      int from_early_init)
{
  clib_error_t *error = 0;
  return error;
}

typedef struct {
  nshfsdev_user_register_t *users;
} nshsfdev_main_t;

nshsfdev_main_t nshsfdev_main;


static int nshfsdev_api_user_register(nshfsdev_user_register_t * user,
			       int *user_index)
{
  nshsfdev_main_t *nsm = &nshsfdev_main;
  nshfsdev_user_register_t *u;
  pool_get (nsm->users, u);
  *u = *user;
  *user_index = u - nsm->users;
  return 0;
}

static int nshfsdev_api_user_unregister(int user_index)
{
  nshsfdev_main_t *nsm = &nshsfdev_main;
  if (!pool_is_free_index(nsm->users, user_index))
    return -1;
  pool_put(nsm->users, pool_elt_at_index(nsm->users, user_index));
  return 0;
}

static uword
nshsfdev_fn (vlib_main_t * vm,
	     vlib_node_runtime_t * node, vlib_frame_t * frame)
{
  u32 n_left_from, *from;
  nshsfdev_main_t *nsm = &nshsfdev_main;

  from = vlib_frame_vector_args (frame);
  n_left_from = frame->n_vectors;

  while (n_left_from > 0)
    {
      u32 pi0;
      vlib_buffer_t *p0;
      nshfsdev_packet_t packet0;
      u32 ui0;
      nshfsdev_user_register_t *u0;

      pi0 = from[0];
      from++;
      n_left_from--;
      p0 = vlib_get_buffer (vm, pi0);
      packet0.data = (char *)vlib_buffer_get_current (p0) - 40;
      packet0.data_len = p0->current_length + 40;
      packet0.data_offset = 40;
      packet0.vpp_opaque = (void *)p0;

      ui0 = vnet_buffer(p0)->sw_if_index[VLIB_TX];
      if (PREDICT_FALSE(pool_is_free_index(nsm->users, ui0))) {
	  vlib_set_next_frame_buffer (vm, node, NSHSFDEV_NEXT_DROP, pi0);
      } else {
	  u0 = pool_elt_at_index(nsm->users, ui0);
	  u0->rx(&packet0, u0->user_opaque);
      }
    }

  return frame->n_vectors;
}

typedef struct {
  u32 a;
} nshsfdev_trace_t;

u8 *
format_nshsfdev_trace (u8 * s, va_list * args)
{
  CLIB_UNUSED (vlib_main_t * vm) = va_arg (*args, vlib_main_t *);
  CLIB_UNUSED (vlib_node_t * node) = va_arg (*args, vlib_node_t *);
  nshsfdev_trace_t *t = va_arg (*args, nshsfdev_trace_t *);
  return format (s, "nshsfdev_trace_t %d", t->a);
}

/** *INDENT-OFF* */
VLIB_REGISTER_NODE (nshsfdev_node, static) =
{
  .function = nshsfdev_fn,
  .name = "nshfsdev",
  .vector_size = sizeof (u32),
  .format_trace = format_nshsfdev_trace,
  .n_errors = NSHSFDEV_N_ERROR,
  .error_strings = nshsfdev_error_strings,
  .n_next_nodes = NSHSFDEV_N_NEXT,
  .next_nodes =
  {
      [NSHSFDEV_NEXT_DROP] = "error-drop"
  },
};
/** *INDENT-ON* */

int nshsfdev_load(u8 *sf_plugin_path)
{
  void *handle, *register_handle;
  int (*fp)(nshsfdev_api_register_t *a);
  int retval;

  handle = dlopen ((char *) sf_plugin_path, RTLD_LAZY);

  if (handle == 0)
    {
      clib_warning ("%s", dlerror ());
      return -1;
    }

  register_handle = dlsym (handle, "sfdev_magic_init");
  if (register_handle == 0)
    {
      dlclose (handle);
      clib_warning ("Plugin missing sfdev_magic_init: %s\n",
		    sf_plugin_path);
      return 1;
    }


  nshsfdev_api_register_t a = {
      .user_register = nshfsdev_api_user_register,
      .user_unregister = nshfsdev_api_user_unregister,
  };
  fp = register_handle;
  clib_warning("Calling with a = %p", &a);
  if ((retval = (*fp)(&a))) {
      clib_warning ("sfdev_magic_init returned %d\n",
		    retval);
      dlclose (handle);
  }

  clib_warning ("Loaded SF plugin: %s", sf_plugin_path);
  return 0;
}

static clib_error_t *
nsh_load_command_fn (vlib_main_t * vm,
		      unformat_input_t * input, vlib_cli_command_t * cmd)
{
  unformat_input_t _line_input, *line_input = &_line_input;
  u8 *sf_plugin_path = 0;
  int retval;

  if (!unformat_user (input, unformat_line_input, line_input)) {
      return clib_error_return (0, "Missing SF plugin path");
  }

  if (!unformat(line_input, "%s", &sf_plugin_path)) {
      unformat_free (line_input);
      return clib_error_return (0, "Missing SF plugin path");
  }

  if ((retval = nshsfdev_load(sf_plugin_path))) {
      unformat_free (line_input);
      return clib_error_return (0, "nshsfdev_load returned %d", retval);
  }

  unformat_free (line_input);
  return NULL;
}

VLIB_CLI_COMMAND (nsh_load_command, static) =
{
  .path = "nsh load",
  .short_help = "nsh load <path>",
  .function = nsh_load_command_fn,
};


static clib_error_t *
nshsf_init_command_fn (vlib_main_t * vm,
		      unformat_input_t * input, vlib_cli_command_t * cmd)
{
  vlib_node_t * nsh_sfc_node = 0;
  nsh_sfc_node = vlib_get_node_by_name (vm, (u8 *)"nsh-input");
  if (!nsh_sfc_node)
    return clib_error_return (0, "Could not find node nsh-input");

  vlib_node_add_next (vm, nsh_sfc_node->index, nshsfdev_node.index);
  vlib_node_add_next (vm, nshsfdev_node.index, nsh_sfc_node->index);
  return NULL;
}

VLIB_CLI_COMMAND (nshsf_init_command, static) =
{
  .path = "nshsf init",
  .short_help = "nshsf init",
  .function = nshsf_init_command_fn,
};

static clib_error_t *
nshsf_show_users_command_fn (vlib_main_t * vm,
		      unformat_input_t * input, vlib_cli_command_t * cmd)
{
  nshsfdev_main_t *nsm = &nshsfdev_main;
  nshfsdev_user_register_t *u;
  pool_foreach (u, nsm->users,
		({
    vlib_cli_output (vm, "[%d] %s\n", u - nsm->users, u->name);
    vlib_cli_output (vm, "     rx:%p poll:%p\n", u->rx, u->poll);
    vlib_cli_output (vm, "     opaque:%p\n", u->user_opaque);
  }));

  return NULL;
}

VLIB_CLI_COMMAND (nshsf_show_users_command, static) =
{
  .path = "nshsf show users",
  .short_help = "nshsf show users",
  .function = nshsf_show_users_command_fn,
};

clib_error_t *nsh_init (vlib_main_t *vm)
{
  nshsfdev_main_t *nsm = &nshsfdev_main;
  nsm->users = 0;
  return 0;
}

VLIB_INIT_FUNCTION(nsh_init);
