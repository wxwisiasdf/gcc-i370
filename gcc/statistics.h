/* Memory and optimization statistics helpers.
   Copyright (C) 2004, 2007, 2008
   Free Software Foundation, Inc.
   Contributed by Cygnus Solutions.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#ifndef GCC_STATISTICS
#define GCC_STATISTICS

#if ! defined GATHER_STATISTICS
#error GATHER_STATISTICS must be defined
#endif

#define GCC_MEM_STAT_ARGUMENTS const char * ARG_UNUSED (_loc_name), int ARG_UNUSED (_loc_line), const char * ARG_UNUSED (_loc_function)
#if GATHER_STATISTICS
#define ALONE_MEM_STAT_DECL GCC_MEM_STAT_ARGUMENTS
#define ALONE_FINAL_MEM_STAT_DECL ALONE_MEM_STAT_INFO
#define ALONE_PASS_MEM_STAT _loc_name, _loc_line,  _loc_function
#define ALONE_FINAL_PASS_MEM_STAT ALONE_PASS_MEM_STAT
#define ALONE_MEM_STAT_INFO __FILE__, __LINE__, __FUNCTION__
#define MEM_STAT_DECL , ALONE_MEM_STAT_DECL
#define FINAL_MEM_STAT_DECL , ALONE_FINAL_MEM_STAT_DECL
#define PASS_MEM_STAT , ALONE_PASS_MEM_STAT
#define FINAL_PASS_MEM_STAT , ALONE_FINAL_PASS_MEM_STAT
#define MEM_STAT_INFO , ALONE_MEM_STAT_INFO
#else
#define ALONE_MEM_STAT_DECL void
#define ALONE_FINAL_MEM_STAT_DECL GCC_MEM_STAT_ARGUMENTS
#define ALONE_PASS_MEM_STAT
#define ALONE_FINAL_PASS_MEM_STAT 0,0,0
#define ALONE_MEM_STAT_INFO
#define MEM_STAT_DECL
#define FINAL_MEM_STAT_DECL , ALONE_FINAL_MEM_STAT_DECL
#define PASS_MEM_STAT
#define FINAL_PASS_MEM_STAT , ALONE_FINAL_PASS_MEM_STAT
#define MEM_STAT_INFO ALONE_MEM_STAT_INFO
#endif

struct function;

/* In statistics.c */
extern void statistics_early_init (void);
extern void statistics_init (void);
extern void statistics_fini (void);
extern void statistics_fini_pass (void);
extern void statistics_counter_event (struct function *, const char *, int);
extern void statistics_histogram_event (struct function *, const char *, int);

#endif
