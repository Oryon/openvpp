

#include <stdio.h>
#include <stdint.h>

#include <nshsfdev/nshsfdev_api.h>

typedef struct {
  uint8_t ver_o_c;
  uint8_t length;
  uint8_t md_type;
  uint8_t next_protocol;
  uint32_t nsp_nsi; // nsp 24 bits, nsi 8 bits
  /* Context headers, always present */
  uint32_t c1; uint32_t c2; uint32_t c3; uint32_t c4;

  /* Optional variable length metadata */
  uint32_t tlvs[0];
} __attribute__ ((packed)) nsh_header_t;

static nshsfdev_api_register_t api;
uint32_t poll_counter = 0;

int sfdev_example_rx(void *user_opaque, nshsfdev_packet_t *packet)
{
  nsh_header_t *hdr = packet->data +packet->data_offset;
  uint8_t *nsi = ((uint8_t *)&(hdr->nsp_nsi)) + 3;
  if (*nsi == 0) {
      printf("Error\n");
    return -1;
  }
  (*nsi)--;
  api.tx(packet);
  return 0;
}

int sfdev_example_poll(void *user_opaque)
{
  poll_counter++;
  //if ((poll_counter & 0xfffff) == 0)
  //  printf("poll counter: %d\n", poll_counter);
  return 0;
}

int sfdev_magic_init(nshsfdev_api_register_t *a) {
  api = *a;
  nshsfdev_user_register_t u;
    u.name = "sf-example";
    u.rx = sfdev_example_rx;
    u.poll = sfdev_example_poll;
    u.user_opaque = (void *)1;
    int index;
    int retval = api.user_register(&u, &index);
  return 0;
}
