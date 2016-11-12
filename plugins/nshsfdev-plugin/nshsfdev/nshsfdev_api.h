
#ifndef NSHFSDEV_API_H
#define NSHFSDEV_API_H

typedef struct {
  char *data;
  int data_len;
  int data_offset;
  void *vpp_opaque;
} nshfsdev_packet_t ;

typedef struct {
  char *name;
  int (*rx)(void *user_opaque, nshfsdev_packet_t *packet);
  int (*poll)(void *user_opaque);
  void *user_opaque;
} nshfsdev_user_register_t;

typedef struct {
  int (*user_register)(nshfsdev_user_register_t * user,
  			       int *user_index);

  int (*user_unregister)(int user_index);

  int (*tx)(nshfsdev_packet_t *packet);

  int (*free)(nshfsdev_packet_t *packet);

  int (*alloc)(nshfsdev_packet_t *packet);
} nshsfdev_api_register_t;

#endif
