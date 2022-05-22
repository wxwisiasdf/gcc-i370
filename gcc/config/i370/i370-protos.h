/* Definitions of target machine for GNU compiler.  System/370 version.
   Copyright (C) 2000 Free Software Foundation, Inc.
   Contributed by Jan Stein (jan@cd.chalmers.se).
   Modified for OS/390 LanguageEnvironment C by Dave Pitts (dpitts@cozx.com)
   Hacked for Linux-ELF/390 by Linas Vepstas (linas@linas.org)

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include "mvs.h"

extern void i370_asm_start (void);
extern void i370_asm_end (void);
extern void i370_asm_globalize_label (FILE *f, const char *name);
extern void i370_asm_label_ref (FILE *f, const char *name);

#ifdef RTX_CODE
extern int i370_branch_dest (rtx insn);
extern int i370_branch_length (rtx insn);
extern int i370_short_branch (rtx insn);
extern int unsigned_jump_follows_p (rtx insn);
#endif /* RTX_CODE */

#ifdef TREE_CODE
extern int handle_pragma (int (*)(void), void (*)(int), const char *);
#endif /* TREE_CODE */

extern void mvs_add_label (int);
extern int mvs_check_label (int);
extern int mvs_check_page (void *, int, int);
extern int mvs_function_check (const char *);
extern void mvs_add_alias (const char *, const char *, int);
extern int mvs_need_alias (const char *);
extern int mvs_get_alias (const char *, char *);
extern int mvs_check_alias (const char *, char *);
extern void check_label_emit (void);
extern void mvs_free_label_list (void);

#ifdef GCC_C_PRAGMA_H
extern void i370_pr_map       (cpp_reader *);
extern void i370_pr_skipit    (cpp_reader *);
extern void i370_pr_linkage   (cpp_reader *);
extern void i370_pr_checkout  (cpp_reader *);
#endif

extern void i370_register_pragmas (void);
