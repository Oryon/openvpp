

#include <stdio.h>
#include <nshsfdev/nshsfdev_api.h>

static nshsfdev_api_register_t api;

int sfdev_magic_init(nshsfdev_api_register_t *a) {
  api = *a;
  nshfsdev_user_register_t u;
    u.name = "sf-example";
    u.rx = 0;
    u.poll = 0;
    u.user_opaque = (void *)1;
    int index;
    int retval = api.user_register(&u, &index);
  return 0;
}
