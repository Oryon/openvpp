
#ifndef NSHSFDEV_H
#define NSHSFDEV_H

#include <vnet/vnet.h>

/**
 * Loads a nsh sf shared object file.
 */
int nshsfdev_load(u8 *sf_plugin_path);

#endif //ifndef NSHSFDEV_H
