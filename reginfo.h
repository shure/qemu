/*
 * Platform registers database.
 *
 * Copyright (c) 2013 Alex Rozenman <alex_rozenman@mentor.com>
 * Copyright (c) 2013 Mentor Graphics Corp.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef REGINFO_H
#define REGINFO_H

#include <stdint.h>

typedef struct RegInfo RegInfo;

struct RegInfo {
  const char *name;
  uint32_t offset;
  uint8_t bitsize;
  const char *type;
  int read_sensitive;
};

void add_reginfo(const char *type_name, const RegInfo *reginfo);
const RegInfo *lookup_reginfo(const char *type_name);
const char *get_platform_xml(void);

#endif
