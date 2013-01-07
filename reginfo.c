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

#include "reginfo.h"
#include "qemu-common.h"
#include "exec/memory.h"
#include "exec/address-spaces.h"

static GHashTable *reginfo_table;

static gboolean str_equal_func(gconstpointer a, gconstpointer b)
{
    return strcmp(a, b) == 0;
}

void add_reginfo(const char *type_name, const RegInfo *reginfo)
{
    if (!reginfo_table) {
        reginfo_table = g_hash_table_new_full(g_str_hash, str_equal_func,
                                              NULL, NULL);
    }
    g_hash_table_insert(reginfo_table, (gpointer)type_name, (gpointer)reginfo);
}

const RegInfo *lookup_reginfo(const char *type_name)
{
    if (!reginfo_table) {
        return 0;
    }
    return g_hash_table_lookup(reginfo_table, (gpointer)type_name);
}


static void generate_reginfo_xml(GString *buf, const char *type_name,
                                 hwaddr base_addr)
{
    const RegInfo *r = lookup_reginfo(type_name);
    if (!r) {
        return;
    }

    g_string_append_printf(buf, "<group name=\"%s_0x%llx\">", type_name,
                           (unsigned long long)base_addr);
    for (; r->name; ++r) {
        g_string_append_printf(buf,
                               "<reg bitsize=\"%u\" name=\"%s\" "
                               "offset=\"0x%llx\" type=\"%s\"/>",
                               r->bitsize, r->name,
                               (unsigned long long)(base_addr + r->offset),
                               r->type);
    }
    g_string_append_printf(buf, "</group>");
}

static void traverse_memory_regions(GString *buf, const MemoryRegion *mr,
                                    hwaddr base)
{
    const MemoryRegion *submr;
    if (!mr || !mr->enabled) {
        return;
    }
    if (!mr->alias) {
        generate_reginfo_xml(buf, mr->name, base + mr->addr);
    }
    QTAILQ_FOREACH(submr, &mr->subregions, subregions_link) {
        traverse_memory_regions(buf, submr, base + mr->addr);
    }
}

static void generate_platform_xml(GString *buf)
{
    g_string_append(buf, "<?xml version=\"1.0\"?>");
    g_string_append(buf, "<!DOCTYPE feature SYSTEM \"gdb-target.dtd\">");
    g_string_append(buf, "<feature name=\"org.gnu.gdb.xfer-spaces\">");
    g_string_append(buf, "<space annex=\"memory\" name=\"HwRegs\">");

    traverse_memory_regions(buf, get_system_memory(), 0);

    g_string_append(buf, "</space>");
    g_string_append(buf, "</feature>");
}

const char *get_platform_xml(void)
{
    static GString *buf;
    if (!buf) {
        buf = g_string_new("");
        generate_platform_xml(buf);
    }
    return (const char*)buf->str;
}
