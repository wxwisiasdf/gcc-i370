/* Common hooks for IBM S/390 and zSeries.
   Copyright (C) 1999-2022 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "diagnostic-core.h"
#include "tm.h"
#include "common/common-target.h"
#include "common/common-target-def.h"
#include "opts.h"
#include "flags.h"

EXPORTED_CONST int processor_flags_table[] =
  {
    /* z900 */   0 | 1,
    /* z900 */   0 | 1,
    /* z900 */   0 | 1,
    /* z900 */   0 | 1,
    /* z900 */   0 | 1,
    /* z900 */   0 | 1,
    /* z900 */   0 | 1,
    /* z900 */   0 | 1,
    /* z900 */   0 | 1,
    /* z900 */   0 | 1,
    /* z900 */   0 | 1,
    /* z900 */   0 | 1,
  };

/* Change optimizations to be performed, depending on the
   optimization level.  */

static const struct default_options s390_option_optimization_table[] =
  {
    /* Enable -fsched-pressure by default when optimizing.  */
    { OPT_LEVELS_1_PLUS, OPT_fsched_pressure, NULL, 1 },

    /* ??? There are apparently still problems with -fcaller-saves.  */
    { OPT_LEVELS_ALL, 0, NULL, 0 },

    /* Use MVCLE instructions to decrease code size if requested.  */
    { OPT_LEVELS_SIZE, 0, NULL, 1 },

    { OPT_LEVELS_NONE, 0, NULL, 0 }
  };

/* Implement TARGET_OPTION_INIT_STRUCT.  */

static void
i370_option_init_struct (struct gcc_options *opts)
{
  /* By default, always emit DWARF-2 unwind info.  This allows debugging
     without maintaining a stack frame back-chain.  */
  opts->x_flag_asynchronous_unwind_tables = 1;

  /* Enable section anchors by default.  */
  opts->x_flag_section_anchors = 1;
}

#undef TARGET_DEFAULT_TARGET_FLAGS
#define TARGET_DEFAULT_TARGET_FLAGS (TARGET_DEFAULT)

#undef TARGET_OPTION_INIT_STRUCT
#define TARGET_OPTION_INIT_STRUCT i370_option_init_struct

struct gcc_targetm_common targetm_common = TARGETM_COMMON_INITIALIZER;
