# Copyright (c) 2016 Cisco and/or its affiliates.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at:
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = g2

AM_CFLAGS = -Wall 

g2_SOURCES =					\
  clib.c					\
  cpel.c					\
  cpel.h					\
  events.c					\
  g2.h						\
  main.c					\
  menu1.c					\
  pointsel.c					\
  props.c					\
  props.h					\
  g2version.c					\
  view1.c

g2_LDADD = $(g2_LIBS) -lvppinfra -lpthread -lm
