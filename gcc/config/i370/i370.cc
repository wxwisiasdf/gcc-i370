/* Subroutines for insn-output.c for System/370.
   Copyright (C) 1989, 1993, 1995, 1997, 1998, 1999, 2000, 2002
   Free Software Foundation, Inc.
   Contributed by Jan Stein (jan@cd.chalmers.se).
   Modified for OS/390 LanguageEnvironment C by Dave Pitts (dpitts@cozx.com)
   Modified for Linux-ELF/390 by Linas Vepstas (linas@linas.org)

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

#define IN_TARGET_CODE 1

#include <cstdio>

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "target.h"
#include "target-globals.h"
#include "rtl.h"
#include "tree.h"
#include "gimple.h"
#include "cfghooks.h"
#include "cfgloop.h"
#include "df.h"
#include "cpplib.h"
#include "memmodel.h"
#include "tm_p.h"
#include "stringpool.h"
#include "attribs.h"
#include "expmed.h"
#include "optabs.h"
#include "regs.h"
#include "emit-rtl.h"
#include "recog.h"
#include "cgraph.h"
#include "diagnostic-core.h"
#include "diagnostic.h"
#include "alias.h"
#include "fold-const.h"
#include "print-tree.h"
#include "stor-layout.h"
#include "varasm.h"
#include "calls.h"
#include "conditions.h"
#include "output.h"
#include "insn-attr.h"
#include "flags.h"
#include "except.h"
#include "dojump.h"
#include "explow.h"
#include "stmt.h"
#include "expr.h"
#include "reload.h"
#include "cfgrtl.h"
#include "cfganal.h"
#include "lcm.h"
#include "cfgbuild.h"
#include "cfgcleanup.h"
#include "debug.h"
#include "langhooks.h"
#include "internal-fn.h"
#include "gimple-iterator.h"
#include "gimple-fold.h"
#include "tree-eh.h"
#include "gimplify.h"
#include "opts.h"
#include "tree-pass.h"
#include "context.h"
#include "builtins.h"
#include "rtl-iter.h"
#include "intl.h"
#include "tm-constrs.h"
#include "tree-vrp.h"
#include "symbol-summary.h"
#include "ipa-prop.h"
#include "ipa-fnsummary.h"
#include "sched-int.h"

#define TARGET_VX_ABI 0

/* maximum length output line */
/* need to allow for people dumping specs */
#define MAX_LEN_OUT 2000

extern FILE *asm_out_file;

/* Label node.  This structure is used to keep track of labels
      on the various pages in the current routine.
   The label_id is the numeric ID of the label,
   The label_page is the page on which it actually appears,
   The first_ref_page is the page on which the true first ref appears.
   The label_addr is an estimate of its location in the current routine,
   The label_first & last_ref are estimates of where the earliest and
      latest references to this label occur.  */

typedef struct label_node
{
  struct label_node *label_next;
  int label_id;
  int label_page;
  int first_ref_page;

  int label_addr;
  int label_first_ref;
  int label_last_ref;
} label_node_t;

struct i370_cc_flags cc_status;

/* if we just inspected a label on another page, we want to
   record that */
static int just_referenced_page = -1;

/* Is 1 when a label has been generated and the base register must be reloaded.  */
int mvs_need_base_reload = 0;

/* Is 1 when an entry point is to be generated.  */
int mvs_need_entry = 0;

/* Is 1 if we have seen main() */
int mvs_gotmain = 0;

int mvs_need_to_globalize = 1;

/* Current function starting base page.  */
int function_base_page;

/* Length of the current page code.  */
int mvs_page_code;

/* Length of the current page literals.  */
int mvs_page_lit;

/* Length of case statement entries */
int mvs_case_code = 0;

/* The desired CSECT name */
char *mvs_csect_name = NULL;

/* Current source module.  */
char *mvs_module = 0;

/* Page number for multi-page functions.  */
int mvs_page_num = 0;

char *mvs_function_name = NULL;
int mvs_function_name_length = 0;

/* First entry point.  */
static int mvs_first_entry = 1;

/* Label node list anchor.  */
static label_node_t *label_anchor = 0;

/* Label node free list anchor.  */
static label_node_t *free_anchor = 0;

/* Assembler source file descriptor.  */
static FILE *assembler_source = 0;

/* Flag that enables position independent code */
int i370_enable_pic = 1;

static label_node_t *mvs_get_label(int);
static void i370_label_scan(void);
#ifdef TARGET_HLASM
static bool i370_hlasm_assemble_integer(rtx, unsigned int, int);
#endif
static void i370_output_function_prologue(FILE *f);
static void i370_output_function_epilogue(FILE *f);
#ifdef TARGET_ALIASES
static int mvs_hash_alias(const char *);
#endif

/* ===================================================== */
/* defines and functions specific to the HLASM assembler */
#ifdef TARGET_HLASM

#define MVS_HASH_PRIME 999983
#if 1 /*defined(HOST_EBCDIC)*/
#define MVS_SET_SIZE 256
#else
#define MVS_SET_SIZE 128
#endif

#ifndef MAX_MVS_LABEL_SIZE
#define MAX_MVS_LABEL_SIZE 8
#endif

#define MAX_LONG_LABEL_SIZE 255

/* Alias node, this structure is used to keep track of aliases to external
   variables. The IBM assembler allows an alias to an external name
   that is longer that 8 characters; but only once per assembly.
   Also, this structure stores the #pragma map info.  */
typedef struct alias_node
{
  struct alias_node *alias_next;
  int alias_emitted;
  int alias_used;
  char alias_name[MAX_MVS_LABEL_SIZE + 1];
  char real_name[MAX_LONG_LABEL_SIZE + 1];
} alias_node_t;

/* Alias node list anchor.  */
static alias_node_t *alias_anchor = 0;

#ifdef TARGET_LE
/* Define the length of the internal MVS function table.  */
#define MVS_FUNCTION_TABLE_LENGTH 32

/* C/370 internal function table.  These functions use non-standard linkage
   and must handled in a special manner.  */
static const char *const mvs_function_table[MVS_FUNCTION_TABLE_LENGTH] =
    {
#if defined(HOST_EBCDIC) /* Changed for EBCDIC collating sequence */
        "ceil", "edc_acos", "edc_asin", "edc_atan", "edc_ata2", "edc_cos",
        "edc_cosh", "edc_erf", "edc_erfc", "edc_exp", "edc_gamm", "edc_lg10",
        "edc_log", "edc_sin", "edc_sinh", "edc_sqrt", "edc_tan", "edc_tanh",
        "fabs", "floor", "fmod", "frexp", "hypot", "jn",
        "j0", "j1", "ldexp", "modf", "pow", "yn",
        "y0", "y1"
#else
        "ceil", "edc_acos", "edc_asin", "edc_ata2", "edc_atan", "edc_cos",
        "edc_cosh", "edc_erf", "edc_erfc", "edc_exp", "edc_gamm", "edc_lg10",
        "edc_log", "edc_sin", "edc_sinh", "edc_sqrt", "edc_tan", "edc_tanh",
        "fabs", "floor", "fmod", "frexp", "hypot", "j0",
        "j1", "jn", "ldexp", "modf", "pow", "y0",
        "y1", "yn"
#endif
};
#endif /* TARGET_LE */

#endif /* TARGET_HLASM */

/* Initialize the GCC target structure.  */
#ifdef TARGET_HLASM
#undef TARGET_ASM_BYTE_OP
#define TARGET_ASM_BYTE_OP ""
#undef TARGET_ASM_ALIGNED_HI_OP
#define TARGET_ASM_ALIGNED_HI_OP ""
#undef TARGET_ASM_ALIGNED_SI_OP
#define TARGET_ASM_ALIGNED_SI_OP ""
#undef TARGET_ASM_INTEGER
#define TARGET_ASM_INTEGER i370_hlasm_assemble_integer
#endif

#define CURRFUNC (IDENTIFIER_POINTER(DECL_NAME(current_function_decl)))

void i370_asm_start(void)
{
  char temp[256];
  const char *cbp, *cfp;
  char *bp;
  if (asm_file_name)
  {
    if (strncmp(asm_file_name, "/tmp", 4) == 0)
      cfp = main_input_filename;
    else
      cfp = asm_file_name;
  }
  else
    cfp = main_input_filename;
  if ((cbp = strrchr(cfp, '/')) == NULL)
    cbp = cfp;
  else
    cbp++;
  while (*cbp == '_')
    cbp++;
  strcpy(temp, cbp);
  if ((bp = strchr(temp, '.')) != NULL)
    *bp = '\0';
  if (strlen(temp) > MAX_MVS_LABEL_SIZE - 1)
    temp[MAX_MVS_LABEL_SIZE - 1] = '\0';
  for (bp = temp; *bp; bp++)
    *bp = ISLOWER(*bp) ? TOUPPER(*bp) : *bp;
  mvs_module = (char *)xmalloc(strlen(temp) + 2);
  strcpy(mvs_module, temp);
#ifdef TARGET_ALIASES
  fprintf(f, "@DATA\tALIAS\tC'@%s'\n", temp);
  fputs("@DATA\tAMODE\tANY\n", f);
  fputs("@DATA\tRMODE\tANY\n", f);
  fputs("@DATA\tCSECT\n", f);
  fputs("\tYREGS\n");
#else
#ifdef TARGET_PDPMAC
  fprintf(asm_out_file, "\tCOPY\tPDPTOP\n");
#endif
  fprintf(asm_out_file, "%s\tCSECT\n", mvs_csect_name ? mvs_csect_name : "");
#endif
}

void i370_asm_end(void)
{
  if (mvs_gotmain)
    fputs("\tEND\t@@MAIN\n", asm_out_file);
  else
    fputs("\tEND\n", asm_out_file);
}

void i370_asm_globalize_label(FILE *f, const char *name)
{
#ifdef TARGET_ALIASES
#ifdef TARGET_DIGNUS
  char temp[MAX_MVS_LABEL_SIZE + 1];
  if (!strcmp(name, "main"))
  {
    fputs("@CRT0\tALIAS\tC'@crt0'\n", f);
    fputs("\tEXTRN\t@CRT0\n", f);
  }
  if (mvs_check_alias(name, temp) == 2)
  {
    fprintf(f, "%s\tALIAS\tC'%s'\n", temp, name);
  }
  if (mvs_need_to_globalize)
  {
    fputs("\tENTRY\t", f);
    assemble_name(f, name);
    fputs("\n", f);
  }
  mvs_need_entry = 1;
#endif
#ifdef TARGET_PDPMAC
  char temp[MAX_MVS_LABEL_SIZE + 1];
  if (!strcmp(name, "main"))
  {
    fputs("@@CRT0\tALIAS\tC'@@crt0'\n", f);
    fputs("\tEXTRN\t@@CRT0\n", f);
  }
  if (mvs_check_alias(name, temp) == 2)
  {
    fprintf(f, "%s\tALIAS\tC'%s'\n", temp, name);
  }
  if (mvs_need_to_globalize)
  {
    fprintf(f, "* X-var %s\n", name);
    fputs("\tENTRY\t", f);
    assemble_name(f, name);
    fputs("\n", f);
  }
  mvs_need_entry = 1;
#endif
#ifdef TARGET_LE
  char temp[MAX_MVS_LABEL_SIZE + 1];
  if (mvs_check_alias(name, temp) == 2)
  {
    fprintf(f, "%s\tALIAS\tC'%s'\n", temp, name);
  }
  fputs("\tENTRY\t", f);
  assemble_name(f, name);
  fputs("\n", f);
#endif
#else /* !TARGET_ALIASES */
#ifdef TARGET_DIGNUS
  char temp[MAX_MVS_LABEL_SIZE + 1];
  if (!strcmp(name, "main"))
  {
    fputs("\tEXTRN\t@CRT0\n", f);
  }
  if (mvs_check_alias(name, temp) == 2)
  {
    fprintf(f, "%s\tALIAS\tC'%s'\n", temp, name);
  }
  if (mvs_need_to_globalize)
  {
    fputs("\tENTRY\t", f);
    assemble_name(f, name);
    fputs("\n", f);
  }
  mvs_need_entry = 1;
#endif
#ifdef TARGET_PDPMAC
#ifdef TARGET_VSE
  char temp[MAX_MVS_LABEL_SIZE + 1];
  if (!strcmp(name, "main"))
  {
    fputs("\tDC\tC'GCCMVS!!'\n", f);
    fputs("\tEXTRN\t@@CRT0\n", f);
    fputs("\tENTRY\t@@MAIN\n", f);
    fputs("@@MAIN\tDS\t0H\n", f);
    fputs("\tBALR\t10,0\n", f);
    fputs("\tUSING\t*,10\n", f);
    fputs("\tL\t10,=V(@@CRT0)\n", f);
    fputs("\tBR\t10\n", f);
    fputs("\tDROP\t10\n", f);
    fputs("\tLTORG\n", f);
    mvs_gotmain = 1;
  }
  if (mvs_check_alias(name, temp) == 2)
  {
    fprintf(f, "%s\tALIAS\tC'%s'\n", temp, name);
  }
  if (mvs_need_to_globalize)
  {
    fprintf(f, "* X-var %s\n", name);
    fputs("\tENTRY\t", f);
    assemble_name(f, name);
    fputs("\n", f);
  }
  mvs_need_entry = 1;
#else
  char temp[MAX_MVS_LABEL_SIZE + 1];
  if (!strcmp(name, "main"))
  {
    fputs("\tCOPY\tPDPMAIN\n", f);
    mvs_gotmain = 1;
  }
  if (mvs_check_alias(name, temp) == 2)
  {
    fprintf(f, "%s\tALIAS\tC'%s'\n", temp, name);
  }
  if (mvs_need_to_globalize)
  {
    fprintf(f, "* X-var %s\n", name);
    fputs("\tENTRY\t", f);
    assemble_name(f, name);
    fputs("\n", f);
  }
  mvs_need_entry = 1;
#endif /* TARGET_VSE */
#endif /* TARGET_PDPMAC */
#endif /* TARGET_ALIASES */
}

void i370_asm_label_ref(FILE *f, const char *name)
{
  char temp[MAX_MVS_LABEL_SIZE + 1];
  if (!mvs_get_alias(name, temp))
    strcpy(temp, name);
  char ch = '@';
  for (char *bp = temp; *bp; bp++)
    *bp = (*bp == '_' ? ch : TOUPPER(*bp));
  fprintf(f, "%s", temp);
}

/* ===================================================== */
/* This is our last chance to clean up before starting to compile.
   We do this to fix up some initializations.   */

void i370_override_options(void)
{
  i370_enable_pic = flag_pic;

  if (mvs_csect_name)
  {
    static char buf[9];
    char *p;

    strncpy(buf, mvs_csect_name, 8);
    p = buf;
    while (*p != '\0')
    {
      *p = TOUPPER((unsigned char)*p);
      p++;
    }
    mvs_csect_name = buf;
  }
}

/* ===================================================== */
/* The following three routines are used to determine whther
   forward branch is on this page, or is a far jump.  We use
   the "length" attr on an insn [(set_atter "length" "4")]
   to store the largest possible code length that insn
   could have.  This gives us a hint of the address of a
   branch destination, and from that, we can work out
   the length of the jump, and whether its on page or not.
 */

/* Return the destination address of a branch.  */

int i370_branch_dest(rtx branch)
{
  rtx dest = SET_SRC(PATTERN(branch));
  /* first, compute the estimated address of the branch target */
  if (GET_CODE(dest) == IF_THEN_ELSE)
    dest = XEXP(dest, 1);
  dest = XEXP(dest, 0);
  int dest_uid = INSN_UID(dest);
  int dest_addr = INSN_ADDRESSES(dest_uid);

  /* next, record the address of this insn as the true addr of first ref */
  {
    rtx label = JUMP_LABEL(branch);
    int labelno = CODE_LABEL_NUMBER(label);

    if (!label || CODE_LABEL != GET_CODE(label))
      gcc_unreachable();

    label_node_t *lp = mvs_get_label(labelno);
    if (-1 == lp->first_ref_page)
      lp->first_ref_page = mvs_page_num;
    just_referenced_page = lp->label_page;
  }
  return dest_addr;
}

int i370_branch_length(rtx insn)
{
  int here = INSN_ADDRESSES(INSN_UID(insn));
  int there = i370_branch_dest(insn);
  return (there - here);
}

int i370_short_branch(rtx insn)
{
  int base_offset = i370_branch_length(insn);
  /* If we just referenced something off-page, then you can
     forget about doing a short branch to it! So for backward
     references, we'll have a page number and can see that it is
     different. For forward references, the page number isn't
     available yet (ie it's still set to -1), so don't use
     this logic on them. */
  if ((just_referenced_page != mvs_page_num) && (just_referenced_page != -1))
  {
    return 0;
  }
  if (0 > base_offset)
  {
    base_offset += mvs_page_code;
  }
  else
  {
    /* avoid bumping into lit pool; use 2x to estimate max possible lits */
    base_offset *= 2;
    base_offset += mvs_page_code + mvs_page_lit;
  }

  /* make a conservative estimate of room left on page */
  if ((MVS_PAGE_CONSERVATIVE > base_offset) && (0 < base_offset))
    return 1;
  return 0;
}

/* The i370_label_scan() routine is supposed to loop over
   all labels and label references in a compilation unit,
   and determine whether all label refs appear on the same
   code page as the label. If they do, then we can avoid
   a reload of the base register for that label.

   Note that the instruction addresses used here are only
   approximate, and make the sizes of the jumps appear
   farther apart then they will actually be.  This makes
   this code far more conservative than it needs to be.
 */

#define I370_RECORD_LABEL_REF(label, addr)     \
  {                                            \
    int labelno = CODE_LABEL_NUMBER(label);    \
    label_node_t *lp = mvs_get_label(labelno); \
    if (addr < lp->label_first_ref)            \
      lp->label_first_ref = addr;              \
    if (addr > lp->label_last_ref)             \
      lp->label_last_ref = addr;               \
  }

static void
i370_label_scan()
{
  int tablejump_offset = 0;

  /* MVS-TODO: Might not be correct to cast */
  for (rtx_insn *insn = get_insns(); insn; insn = NEXT_INSN(insn))
  {
    int here = INSN_ADDRESSES(INSN_UID(insn));
    enum rtx_code code = GET_CODE(insn);

    /* ??? adjust for tables embedded in the .text section that
     * the compiler didn't take into account */
    here += tablejump_offset;
    INSN_ADDRESSES(INSN_UID(insn)) = here;

    /* check to see if this insn is a label ...  */
    if (CODE_LABEL == code)
    {
      int labelno = CODE_LABEL_NUMBER(insn);

      label_node_t *lp = mvs_get_label(labelno);
      lp->label_addr = here;
#if 0
      /* Supposedly, labels are supposed to have circular
        lists of label-refs that reference them, 
        setup in flow.c, but this does not appear to be the case.  */
      rtx labelref = LABEL_REFS (insn);
      rtx ref = labelref;
      do 
      {
        rtx linsn = CONTAINING_INSN(ref);
        ref =  LABEL_NEXTREF(ref);
      } while (ref && (ref != labelref));
#endif
    }
    else if (JUMP_INSN == code)
    {
      rtx label = JUMP_LABEL(insn);

      /* If there is no label for this jump, then this
         had better be a ADDR_VEC or an ADDR_DIFF_VEC
         and there had better be a vector of labels.  */
      if (!label)
      {
        rtx body = PATTERN(insn);
        if (ADDR_VEC == GET_CODE(body))
        {
          for (int j = 0; j < XVECLEN(body, 0); j++)
          {
            rtx lref = XVECEXP(body, 0, j);
            if (LABEL_REF != GET_CODE(lref))
              gcc_unreachable();
            label = XEXP(lref, 0);
            if (CODE_LABEL != GET_CODE(label))
              gcc_unreachable();
            tablejump_offset += 4;
            here += 4;
            I370_RECORD_LABEL_REF(label, here);
          }
          /* finished with the vector go do next insn */
          continue;
        }
        else if (ADDR_DIFF_VEC == GET_CODE(body))
        {
          /* XXX hack alert.
             Right now, we leave this as a no-op, but strictly speaking,
             this is incorrect.  It is possible that a table-jump
             driven off of a relative address could take us off-page,
             to a place where we need to reload the base reg.  So really,
             we need to examing both labels, and compare thier values
             to the current basereg value.

             More generally, this brings up a troubling issue overall:
             what happens if a tablejump is split across two pages? I do
             not beleive that this case is handled correctly at all, and
             can only lead to horrible results if this were to occur.

             However, the current situation is not any worse than it was
             last week, and so we punt for now.  */

          debug_rtx(insn);
          for (int j = 0; j < XVECLEN(body, 0); j++)
          {
          }
          /* finished with the vector go do next insn */
          continue;
        }
        else
        {
          /* XXX hack alert.
             Compiling the exception handling (L_eh) in libgcc2.a will trip
             up right here, with something that looks like
             (set (pc) (mem:SI (plus:SI (reg/v:SI 1 r1) (const_int 4))))
                {indirect_jump}
             I'm not sure of what leads up to this, but it looks like
             the makings of a long jump which will surely get us into trouble
             because the base & page registers don't get reloaded.  For now
             I'm not sure of what to do ... again we punt ... we are not worse
             off than yesterday.  */

          /* print_rtl_single (stdout, insn); */
          /* debug_rtx (insn); */
          /* gcc_unreachable(); */
          continue;
        }
      }
      else
      {
        /* At this point, this jump_insn had better be a plain-old
           ordinary one, grap the label id and go */
        if (CODE_LABEL != GET_CODE(label))
          gcc_unreachable();
        I370_RECORD_LABEL_REF(label, here);
      }
    }

    /* Sometimes, we take addresses of labels and use them
       as instruction operands ... these show up as REG_NOTES */
    else if (INSN == code)
    {
      if ('i' == GET_RTX_CLASS(code))
      {
        for (rtx note = REG_NOTES(insn); note; note = XEXP(note, 1))
        {
          /* MVS-TODO: Ambigous */
          if (REG_LABEL_TARGET == REG_NOTE_KIND(note))
          {
            rtx label = XEXP(note, 0);
            if (!label || CODE_LABEL != GET_CODE(label))
              gcc_unreachable();

            I370_RECORD_LABEL_REF(label, here);
          }
        }
      }
    }
  }
}

/* ===================================================== */

/* Emit reload of base register if indicated.  This is to eliminate multiple
   reloads when several labels are generated pointing to the same place
   in the code.

   The table of base register values is created at the end of the function.
   The MVS/OE/USS/HLASM version keeps this table in the text section, and
   it looks like the following:
      PGT0 EQU *
      DC A(PG0)
      DC A(PG1)

   The ELF version keeps the base register table in either the text or the
   data section, depending on the setting of the i370_enable_pic flag.
   Disabling this flag frees r12 for general purpose use, but makes the
   code non-relocatable.  The non-pic table resemble the mvs-style table.
   The pic table stores values for both r3 (the register used for branching)
   and r12 (the register to index the literal pool, also in the data section).
   Thus, the ELF pic version has twice as many entries, and double the offset.

     .LPGT0:          // PGT0 EQU *
     .long .LPG0      // DC A(PG0)
     .long .LPOOL0
     .long .LPG1      // DC A(PG1)
     .long .LPOOL1

  Note that the functin prologue loads the page addressing register:
      L       PAGE_REGNUM,=A(.LPGT0)

  The ELF version then stores this value at 0(r13), so that its always
  accessible. This frees up r4 for general register allocation; whereas
  the MVS version is stuck with r4.

  Note that this addressing scheme breaks down when a single subroutine
  has more than twelve MBytes of code or so for non-pic, and 6MB for pic.
  Its hard to imagine under what circumstances a single subroutine would
  ever get that big ...
 */

#ifdef TARGET_HLASM
void check_label_emit()
{
  if (mvs_need_base_reload)
  {
    mvs_need_base_reload = 0;
    mvs_page_code += 4;
    fprintf(assembler_source, "\tL\t%i,%i(,%i)\n", BASE_REGNUM, (mvs_page_num - function_base_page) * 4, PAGE_REGNUM);
  }
}
#endif /* TARGET_HLASM */

/* Add the label to the current page label list.  If a free element is available
   it will be used for the new label.  Otherwise, a label element will be
   allocated from memory.
   ID is the label number of the label being added to the list.  */

static label_node_t *
mvs_get_label(int id)
{
  label_node_t *lp;

  /* first, lets see if we already go one, if so, use that.  */
  for (lp = label_anchor; lp; lp = lp->label_next)
  {
    if (lp->label_id == id)
      return lp;
  }

  /* not found, get a new one */
  if (free_anchor)
  {
    lp = free_anchor;
    free_anchor = lp->label_next;
  }
  else
  {
    lp = (label_node_t *)xmalloc(sizeof(label_node_t));
  }

  /* initialize for new label */
  lp->label_id = id;
  lp->label_page = -1;
  lp->label_next = label_anchor;
  lp->label_first_ref = 2000123123;
  lp->label_last_ref = -1;
  lp->label_addr = -1;
  lp->first_ref_page = -1;
  label_anchor = lp;
  return lp;
}

static int _internal_id = 0;

void mvs_add_label(int id)
{
  label_node_t *lp;
  int fwd_distance;

  id = _internal_id;
  _internal_id++;

  lp = mvs_get_label(id);
  lp->label_page = mvs_page_num;

/* Note that without this, some case statements are
     not generating correct code, e.g. case '{' in
     do_spec_1 in gcc.c */
#if 1
  if (mvs_page_num != function_base_page)
  {
    mvs_need_base_reload++;
    return;
  }
#endif

  /* OK, we just saw the label.  Determine if this label
   * needs a reload of the base register */
  if ((-1 != lp->first_ref_page) &&
      (lp->first_ref_page != mvs_page_num))
  {
    /* Yep; the first label_ref was on a different page.  */
    mvs_need_base_reload++;
    return;
  }

  /* Hmm.  Try to see if the estimated address of the last
     label_ref is on the current page.  If it is, then we
     don't need a base reg reload.  Note that this estimate
     is very conservatively handled; we'll tend to have
     a good bit more reloads than actually needed.  Someday,
     we should tighten the estimates (which are driven by
     the (set_att "length") insn attibute.

     Currently, we estimate that number of page literals
     same as number of insns, which is a vast overestimate,
     esp that the estimate of each insn size is its max size.  */

  /* if latest ref comes before label, we are clear */
  if (lp->label_last_ref < lp->label_addr)
    return;

  fwd_distance = lp->label_last_ref - lp->label_addr;

  if (mvs_page_code + 2 * fwd_distance + mvs_page_lit < MVS_PAGE_CONSERVATIVE)
    return;

  mvs_need_base_reload++;
}

/* Check to see if the label is in the list and in the current
   page.  If not found, we have to make worst case assumption
   that label will be on a different page, and thus will have to
   generate a load and branch on register.  This is rather
   ugly for forward-jumps, but what can we do? For backward
   jumps on the same page we can branch directly to address.
   ID is the label number of the label being checked.  */

int mvs_check_label(int id)
{
  label_node_t *lp;

  for (lp = label_anchor; lp; lp = lp->label_next)
  {
    if (lp->label_id == id)
    {
      if (lp->label_page == mvs_page_num)
      {
        return 1;
      }
      else
      {
        return 0;
      }
    }
  }
  return 0;
}

/* The label list for the current page freed by linking the list onto the free
   label element chain.  */
void mvs_free_label_list()
{
  if (label_anchor)
  {
    label_node_t *last_lp = label_anchor;
    while (last_lp->label_next)
      last_lp = last_lp->label_next;
    last_lp->label_next = free_anchor;
    free_anchor = label_anchor;
  }
  label_anchor = 0;
}

/* ====================================================================== */
/* If the page size limit is reached a new code page is started, and the base
   register is set to it.  This page break point is counted conservatively,
   most literals that have the same value are collapsed by the assembler.
   True is returned when a new page is started.
   FILE is the assembler output file descriptor.
   CODE is the length, in bytes, of the instruction to be emitted.
   LIT is the length of the literal to be emitted.  */

#ifdef TARGET_HLASM
int mvs_check_page(void *_file, int code, int lit)
{
  FILE *file = (FILE *)_file;
  if (file)
    assembler_source = file;

  if (mvs_page_code + code + mvs_page_lit + lit > MAX_MVS_PAGE_LENGTH)
  {
    /* no need to dump literals if we're at the end of
       a case statement - they will already have been
 dumped prior to the jump table generation. */
    if (mvs_case_code == 0)
    {
      fprintf(assembler_source, "\tB\t@@PGE%i\n", mvs_page_num);
      fprintf(assembler_source, "\tDS\t0F\n");
      fprintf(assembler_source, "\tLTORG\n");
    }
    fprintf(assembler_source, "\tDS\t0F\n");
    fprintf(assembler_source, "@@PGE%i\tEQU\t*\n", mvs_page_num);
    fprintf(assembler_source, "\tDROP\t%i\n", BASE_REGNUM);
    mvs_page_num++;
    fprintf(assembler_source, "\tBALR\t%i,0\n", BASE_REGNUM);
    fprintf(assembler_source, "\tUSING\t*,%i\n", BASE_REGNUM);
    fprintf(assembler_source, "@@PG%i\tEQU\t*\n", mvs_page_num);
    mvs_page_code = code;
    mvs_page_lit = lit;
    return 1;
  }
  mvs_page_code += code;
  mvs_page_lit += lit;
  return 0;
}
#endif /* TARGET_HLASM */

/* ===================================================== */
/* defines and functions specific to the HLASM assembler */
#ifdef TARGET_HLASM

/* Check for C/370 runtime function, they don't use standard calling
   conventions.  True is returned if the function is in the table.
   NAME is the name of the current function.  */
int mvs_function_check(const char *name)
{
#ifdef TARGET_LE
  int lower, middle, upper;
  int i;

  lower = 0;
  upper = MVS_FUNCTION_TABLE_LENGTH - 1;
  while (lower <= upper)
  {
    middle = (lower + upper) / 2;
    i = strcmp(name, mvs_function_table[middle]);
    if (i == 0)
      return 1;
    if (i < 0)
      upper = middle - 1;
    else
      lower = middle + 1;
  }
#endif
  return 0;
}

/* Generate a hash for a given key.  */

#ifdef TARGET_ALIASES
static int
mvs_hash_alias(const char *key)
{
  int h;
  int i;
  int l = strlen(key);

  h = (unsigned char)MAP_OUTCHAR(key[0]);
  for (i = 1; i < l; i++)
    h = ((h * MVS_SET_SIZE) + (unsigned char)MAP_OUTCHAR(key[i])) % MVS_HASH_PRIME;
  return (h);
}
#endif

/* Add the alias to the current alias list.  */

void mvs_add_alias(const char *realname, const char *aliasname, int emitted)
{
  alias_node_t *ap = (alias_node_t *)xmalloc(sizeof(alias_node_t));
  if (strlen(realname) > MAX_LONG_LABEL_SIZE)
  {
    warning(0, "real name is too long - alias ignored");
    return;
  }
  if (strlen(aliasname) > MAX_MVS_LABEL_SIZE)
  {
    warning(0, "alias name is too long - alias ignored");
    return;
  }

  strcpy(ap->real_name, realname);
  strcpy(ap->alias_name, aliasname);
  ap->alias_emitted = emitted;
  ap->alias_used = 0 /* FALSE */;
  ap->alias_next = alias_anchor;
  alias_anchor = ap;
}

/* Check to see if the name needs aliasing. ie. the name is either:
     1. Longer than 8 characters
     2. Contains an underscore
     3. Is mixed case */

int mvs_need_alias(const char *realname)
{
#if defined(TARGET_DIGNUS) || defined(TARGET_PDPMAC)
  return 1;
#else
  if (mvs_function_check(realname))
    return 0;
#if 0
   if (!strcmp (realname, "gccmain"))
     return 0;
   if (!strcmp (realname, "main"))
     return 0;
#endif
  int i, j = strlen(realname);
  if (j > MAX_MVS_LABEL_SIZE)
    return 1;
  if (strchr(realname, '_') != 0)
    return 1;
  if (ISUPPER(realname[0]))
  {
    for (i = 1; i < j; i++)
    {
      if (ISLOWER(realname[i]))
        return 1;
    }
  }
  else
  {
    for (i = 1; i < j; i++)
    {
      if (ISUPPER(realname[i]))
        return 1;
    }
  }
  return 0;
#endif
}

/* Mark an alias as used as an external.  */

void mvs_mark_alias(const char *realname)
{
  for (alias_node_t *ap = alias_anchor; ap; ap = ap->alias_next)
  {
    if (!strcmp(ap->real_name, realname))
    {
      ap->alias_used = 1;
      return;
    }
  }
  return;
}

/* Dump any used aliases that have been emitted.  */

int mvs_dump_alias(FILE *f)
{
  for (alias_node_t *ap = alias_anchor; ap; ap = ap->alias_next)
  {
    if (ap->alias_used && !ap->alias_emitted)
    {
      fprintf(f, "%s\tALIAS\tC'%s'\n",
              ap->alias_name,
              ap->real_name);
    }
  }
  return 0;
}

/* https://stackoverflow.com/questions/2624192/good-hash-function-for-strings */
int mvs_hash_alias(const char *realname)
{
  size_t len = strlen(realname);
  int hash = 7;
  for (int i = 0; i < len; i++)
    hash = hash * 31 + realname[i];
  return hash;
}

/* Get the alias from the list.
   If 1 is returned then it's in the alias list, 0 if it was not */

int mvs_get_alias(const char *realname, char *aliasname)
{
#ifdef TARGET_ALIASES
  for (alias_node_t *ap = alias_anchor; ap; ap = ap->alias_next)
  {
    if (!strcmp(ap->real_name, realname))
    {
      strcpy(aliasname, ap->alias_name);
      return 1;
    }
  }
  if (mvs_need_alias(realname))
  {
    char c1, c2;

    c1 = realname[0];
    c2 = realname[1];
    if (ISLOWER(c1))
      c1 = TOUPPER(c1);
    else if (c1 == '_')
      c1 = 'A';
    if (ISLOWER(c2))
      c2 = TOUPPER(c2);
    else if (c2 == '_' || c2 == '\0')
      c2 = '#';

    sprintf(aliasname, "%c%c%06d", c1, c2, mvs_hash_alias(realname));
    mvs_add_alias(realname, aliasname, 0);
    return 1;
  }
#else
  if (strlen(realname) > MAX_MVS_LABEL_SIZE)
  {
    size_t len = strlen(realname);
    if(len >= MAX_MVS_LABEL_SIZE)
      len = MAX_MVS_LABEL_SIZE;
    strncpy(aliasname, realname, MAX_MVS_LABEL_SIZE);
    aliasname[len] = '\0';
    return 1;
  }
#endif
  return 0;
}

/* Check to see if the alias is in the list.
   If 1 is returned then it's in the alias list, 2 it was emitted  */

int mvs_check_alias(const char *realname, char *aliasname)
{
#ifdef TARGET_ALIASES
  for (alias_node_t *ap = alias_anchor; ap; ap = ap->alias_next)
  {
    if (!strcmp(ap->real_name, realname))
    {
      int rc = (ap->alias_emitted == 1) ? 1 : 2;
      strcpy(aliasname, ap->alias_name);
      ap->alias_emitted = 1;
      return rc;
    }
  }
  if (mvs_need_alias(realname))
  {
    char c1, c2;

    c1 = realname[0];
    c2 = realname[1];
    if (ISLOWER(c1))
      c1 = TOUPPER(c1);
    else if (c1 == '_')
      c1 = 'A';
    if (ISLOWER(c2))
      c2 = TOUPPER(c2);
    else if (c2 == '_' || c2 == '\0')
      c2 = '#';

    sprintf(aliasname, "%c%c%06d", c1, c2, mvs_hash_alias(realname));
    mvs_add_alias(realname, aliasname, 0);
    alias_anchor->alias_emitted = 1;
    return 2;
  }
#else
  if (strlen(realname) > MAX_MVS_LABEL_SIZE)
  {
    size_t len = strlen(realname);
    if(len >= MAX_MVS_LABEL_SIZE)
      len = MAX_MVS_LABEL_SIZE;
    strncpy(aliasname, realname, MAX_MVS_LABEL_SIZE);
    aliasname[len] = '\0';
    return 1;
  }
#endif
  return 0;
}

#endif /* TARGET_HLASM */

/* ===================================================== */

/* Some remarks about unsigned_jump_follows_p():
   gcc is built around the assumption that branches are signed
   or unsigned, whereas the 370 doesn't care; its the compares that
   are signed or unsigned.  Thus, we need to somehow know if we
   need to do a signed or an unsigned compare, and we do this by
   looking ahead in the instruction sequence until we find a jump.
   We then note whether this jump is signed or unsigned, and do the
   compare appropriately.  Note that we have to scan ahead indefinitley,
   as the gcc optimizer may insert any number of instructions between
   the compare and the jump.

   Note that using conditional branch expanders seems to be be a more
   elegant/correct way of doing this.   See, for instance, the Alpha
   cmpdi and bgt patterns.  Note also that for the i370, various
   arithmetic insn's set the condition code as well.

   The unsigned_jump_follows_p() routine  returns a 1 if the next jump
   is unsigned.  INSN is the current instruction. We err on the side
   of assuming unsigned, so there are a lot of return 1. */

int unsigned_jump_follows_p(rtx x)
{
  rtx_insn *insn = NULL;
  bool first = true;
  while (1)
  {
    if (first)
    {
      insn = get_last_insn();
      first = false;
    }
    else
    {
      gcc_assert(insn != NULL);
      insn = NEXT_INSN(insn);
    }

    /* Exit out of the loop if no instruction is found */
    if (insn == NULL)
      return (1);

    if (GET_CODE(insn) != JUMP_INSN)
      continue;

    rtx tmp_insn = PATTERN(insn);
    if (tmp_insn == NULL)
      continue;
    if (GET_CODE(tmp_insn) != SET || GET_CODE(XEXP(tmp_insn, 0)) != PC)
      continue;

    tmp_insn = XEXP(tmp_insn, 1);
    if (GET_CODE(tmp_insn) != IF_THEN_ELSE)
      continue;
  }

  /* if we got to here, this instruction is a jump.  Is it signed? */
  rtx tmp_insn = XEXP(tmp_insn, 0);
  enum rtx_code coda = GET_CODE(tmp_insn);

  /* if we get an equal or not equal, either comparison
      will work. What we're really interested in what happens
      after that. So check one more instruction to see if
      anything comes up. */
  if ((coda == EQ) || (coda == NE))
  {
    /* MVS-TODO: Might not be correct to cast */
    insn = NEXT_INSN(insn);
    if (!insn)
      return (1);

    if (GET_CODE(insn) != JUMP_INSN)
    {
      /* skip any labels or notes or non-branching
          instructions, looking to see if there's a
          branch ahead */
      while (GET_CODE(insn) != JUMP_INSN)
      {
        if ((GET_CODE(insn) != CODE_LABEL) && (GET_CODE(insn) != NOTE) && (GET_CODE(insn) != INSN) && (GET_CODE(insn) != JUMP_INSN))
          return (1);
        /* MVS-TODO: Might not be correct to cast */
        insn = NEXT_INSN(insn);
        if (!insn)
          return (1);
      }
    }

    tmp_insn = PATTERN(insn);
    if (tmp_insn == NULL)
      gcc_unreachable();

    if (GET_CODE(tmp_insn) != SET)
      return (1);

    if (GET_CODE(XEXP(tmp_insn, 0)) != PC)
      return (1);

    tmp_insn = XEXP(tmp_insn, 1);
    if (GET_CODE(tmp_insn) != IF_THEN_ELSE)
      return (1);

    tmp_insn = XEXP(tmp_insn, 0);
    coda = GET_CODE(tmp_insn);
  }

  /* if we got to here, this instruction is a jump.  Is it signed? */
  return coda != GE && coda != GT && coda != LE && coda != LT;
}

#ifdef TARGET_HLASM

/* Target hook for assembling integer objects.  This version handles all
   objects when TARGET_HLASM is defined.  */

static bool
i370_hlasm_assemble_integer(rtx x, unsigned int size, int aligned_p)
{
  const char *int_format = NULL;
  int intmask;

  if (aligned_p)
    switch (size)
    {
    case 1:
      int_format = "\tDC\tX'%02X'\n";
      intmask = 0xFF;
      break;

    case 2:
      int_format = "\tDC\tX'%04X'\n";
      intmask = 0xFFFF;
      break;

    case 4:
      if (GET_CODE(x) == CONST_INT)
      {
        fputs("\tDC\tF'", asm_out_file);
        output_addr_const(asm_out_file, x);
        fputs("'\n", asm_out_file);
      }
      else
      {
        if (GET_CODE(x) == CONST && GET_CODE(XEXP(XEXP(x, 0), 0)) == SYMBOL_REF && SYMBOL_REF_FLAG(XEXP(XEXP(x, 0), 0)))
        {
          const char *fname;
          typedef struct _entnod
          {
            char *data;
            struct _entnod *next;
          } entnod;
          static entnod *enstart = NULL;
          entnod **en;

          fname = XSTR((XEXP(XEXP(x, 0), 0)), 0);
          en = &enstart;
          while (*en != NULL)
          {
            if (strcmp((*en)->data, fname) == 0)
              break;
            en = &((*en)->next);
          }
          if (*en == NULL)
          {
            *en = (entnod *)xmalloc(sizeof(entnod));
            (*en)->data = (char *)xmalloc(strlen(fname) + 1);
            strcpy((*en)->data, fname);
            (*en)->next = NULL;
            fputs("\tEXTRN\t", asm_out_file);
            assemble_name(asm_out_file,
                          XSTR((XEXP(XEXP(x, 0), 0)), 0));
            fputs("\n", asm_out_file);
          }
        }
        if (SYMBOL_REF_FLAG(x))
        {
          fputs("\tDC\tV(", asm_out_file);
        }
        else
        {
          fputs("\tDC\tA(", asm_out_file);
        }
        output_addr_const(asm_out_file, x);
        fputs(")\n", asm_out_file);
      }
      return true;
    }

  if (int_format && GET_CODE(x) == CONST_INT)
  {
    fprintf(asm_out_file, int_format, INTVAL(x) & intmask);
    return true;
  }
  return default_assemble_integer(x, size, aligned_p);
}

/* Generate the assembly code for function entry.  FILE is a stdio
   stream to output the code to.  SIZE is an int: how many units of
   temporary storage to allocate.

   Refer to the array `regs_ever_live' to determine which registers to
   save; `regs_ever_live[I]' is nonzero if register number I is ever
   used in the function.  This function is responsible for knowing
   which registers should not be saved even if used.  */

static void
i370_output_function_prologue(FILE *f)
{
  HOST_WIDE_INT frame_usage = STACK_FRAME_BASE;
  if (cfun != NULL && cfun->su != NULL)
  {
    frame_usage += current_function_static_stack_size;
  }

  /* Don't print stack and args in PDPMAC as it makes the
   comment too long */
#ifdef TARGET_PDPMAC
  fprintf(f, "* %c-func %s prologue\n",
          mvs_need_entry ? 'X' : 'S',
          CURRFUNC);
#else
  fprintf(f, "* Function %s prologue: stack = %i, args = %i\n",
          mvs_function_name,
          frame_usage,
          current_function_static_stack_size);
#endif
  if (mvs_first_entry)
  {
#ifdef TARGET_ALIASES
    fprintf(f, "@CODE\tALIAS\tC'@%s'\n", mvs_module);
    fputs("@CODE\tAMODE\tANY\n", f);
    fputs("@CODE\tRMODE\tANY\n", f);
    fputs("@CODE\tCSECT\n", f);
#elif !defined(TARGET_PDPMAC)
    fprintf(f, "@%s\tCSECT\n", mvs_module);
#endif
    mvs_first_entry = 0;
  }
#ifdef TARGET_MACROS
#if defined(TARGET_DIGNUS) || defined(TARGET_PDPMAC)
  assemble_name(f, mvs_function_name);
  const char *eprol_macname =
#ifdef TARGET_DIGNUS
      "DCCPRLG";
#elif defined TARGET_PDPMAC
      "PDPPRLG";
#else
      NULL;
#endif

  if (eprol_macname != NULL)
  {
    fprintf(f, "\t%s CINDEX=%i,FRAME=%ld,BASER=%i,ENTRY=%s\n", eprol_macname, mvs_page_num, frame_usage, BASE_REGNUM, mvs_need_entry ? "YES" : "NO");
  }

  char temp[MAX_MVS_LABEL_SIZE + 1];
  if (!mvs_get_alias(mvs_function_name, temp))
    strcpy(temp, mvs_function_name);
  char ch = '@';
  for (char *bp = temp; *bp; bp++)
    *bp = (*bp == '_' ? ch : TOUPPER(*bp));
  fprintf(f, "%-8sEQU *", temp);

  fprintf(f, "\tB\t@@FEN%i\n", mvs_page_num);
#ifdef TARGET_DIGNUS
  fprintf(f, "@FRAMESIZE_%i DC F'%i'\n",
          mvs_page_num,
          frame_usage);
#endif
#ifdef TARGET_PDPMAC
  fprintf(f, "\tLTORG\n");
#endif
  fprintf(f, "@@FEN%i\tEQU\t*\n", mvs_page_num);
  fprintf(f, "\tDROP\t%i\n", BASE_REGNUM);
  fprintf(f, "\tBALR\t%i,0\n", BASE_REGNUM);
  fprintf(f, "\tUSING\t*,%i\n", BASE_REGNUM);
#endif
#ifdef TARGET_LE
  assemble_name(f, mvs_function_name);
  fprintf(f, "\tEDCPRLG USRDSAL=%i,BASEREG=%i\n",
          STACK_FRAME_BASE + l,
          BASE_REGNUM);
#endif

#else /* TARGET_MACROS != 1 */

#if defined(TARGET_LE)
  {
    static int function_label_index = 1;
    static int function_first = 0;
    static int function_year, function_month, function_day;
    static int function_hour, function_minute, function_second;
    if (!function_first)
    {
      struct tm *function_time;
      time_t lcltime;
      time(&lcltime);
      function_time = localtime(&lcltime);
      function_year = function_time->tm_year + 1900;
      function_month = function_time->tm_mon + 1;
      function_day = function_time->tm_mday;
      function_hour = function_time->tm_hour;
      function_minute = function_time->tm_min;
      function_second = function_time->tm_sec;
    }
    fprintf(f, "FDSE%03d\tDSECT\n", function_label_index);
    fprintf(f, "\tDS\tD\n");
    fprintf(f, "\tDS\tCL(%i)\n", STACK_POINTER_OFFSET + l + current_function_outgoing_args_size);
    fprintf(f, "\tORG\tFDSE%03d\n", function_label_index);
    fprintf(f, "\tDS\tCL(120+8)\n");
    fprintf(f, "\tORG\n");
    fprintf(f, "\tDS\t0D\n");
    fprintf(f, "FDSL%03d\tEQU\t*-FDSE%03d-8\n", function_label_index,
            function_label_index);
    fprintf(f, "\tDS\t0H\n");
#ifdef TARGET_ALIASES
    fprintf(f, "@CODE\tCSECT\n");
#else
    fprintf(f, "@%s\tCSECT\n", mvs_module);
#endif
    fprintf(f, "\tUSING\t*,15\n");
    assemble_name(f, mvs_function_name);
    fprintf(f, "\tB\t@@FENT%03d\n", function_label_index);
    fprintf(f, "\tDC\tAL1(FNAM%03d+4-*)\n", function_label_index);
    fprintf(f, "\tDC\tX'CE',X'A0',AL1(16)\n");
    fprintf(f, "\tDC\tAL4(FPPA%03d)\n", function_label_index);
    fprintf(f, "\tDC\tAL4(0)\n");
    fprintf(f, "\tDC\tAL4(FDSL%03d)\n", function_label_index);
    fprintf(f, "FNAM%03d\tEQU\t*\n", function_label_index);
    fprintf(f, "\tDC\tAL2(%i),C'%s'\n", strlen(mvs_function_name),
            mvs_function_name);
    fprintf(f, "FPPA%03d\tDS\t0F\n", function_label_index);
    fprintf(f, "\tDC\tX'03',X'00',X'33',X'00'\n");
    fprintf(f, "\tDC\tV(CEESTART)\n");
    fprintf(f, "\tDC\tAL4(0)\n");
    fprintf(f, "\tDC\tAL4(FTIM%03d)\n", function_label_index);
    fprintf(f, "FTIM%03d\tDS\t0F\n", function_label_index);
    fprintf(f, "\tDC\tCL4'%i',CL4'%02d%02d',CL6'%02d%02d00'\n",
            function_year, function_month, function_day,
            function_hour, function_minute);
    fprintf(f, "\tDC\tCL2'01',CL4'0100'\n");
    fprintf(f, "@@FENT%03d\tDS\t0H\n", function_label_index);
    fprintf(f, "\tSTM\t14,12,12(13)\n");
    fprintf(f, "\tL\t2,76(,13)\n");
    fprintf(f, "\tL\t0,16(,15)\n");
    fprintf(f, "\tALR\t0,2\n");
    fprintf(f, "\tCL\t0,12(,12)\n");
    fprintf(f, "\tBNH\t*+10\n");
    fprintf(f, "\tL\t15,116(,12)\n");
    fprintf(f, "\tBALR\t14,15\n");
    fprintf(f, "\tL\t15,72(,13)\n");
    fprintf(f, "\tSTM\t15,0,72(2)\n");
    fprintf(f, "\tMVI\t0(2),X'10'\n");
    fprintf(f, "\tST\t2,8(,13)\n ");
    fprintf(f, "\tST\t13,4(,2)\n ");
    fprintf(f, "\tLR\t13,2\n");
    fprintf(f, "\tDROP\t15\n");
    fprintf(f, "\tBALR\t%i,0\n", BASE_REGNUM);
    fprintf(f, "\tUSING\t*,%i\n", BASE_REGNUM);
    function_first = 1;
    function_label_index++;
  }
#endif /* TARGET_LE */

#endif /* TARGET_MACROS */
  fprintf(f, "@@PG%i\tEQU\t*\n", mvs_page_num);
  fprintf(f, "\tLR\t11,1\n");
  fprintf(f, "\tL\t%i,=A(@@PGT%i)\n", PAGE_REGNUM, mvs_page_num);
  fprintf(f, "* Function %s code\n", CURRFUNC);

  mvs_free_label_list();
  mvs_page_code = 6;
  mvs_page_lit = 4;
  mvs_check_page(f, 0, 0);
  function_base_page = mvs_page_num;

  /* find all labels in this routine */
  i370_label_scan();
}
#endif /* TARGET_HLASM */

/* This function generates the assembly code for function exit.
   Args are as for output_function_prologue ().

   The function epilogue should not depend on the current stack
   pointer!  It should use the frame pointer only.  This is mandatory
   because of alloca; we also take advantage of it to omit stack
   adjustments before returning.  */

void i370_output_function_epilogue(FILE *file)
{
  check_label_emit();
  mvs_check_page(file, 14, 0);
  fprintf(file, "* Function %s epilogue\n", CURRFUNC);
  mvs_page_num++;

#ifdef TARGET_MACROS

#ifdef TARGET_DIGNUS
  fprintf(file, "\tDCCEPIL\n");
#endif
#ifdef TARGET_PDPMAC
  fprintf(file, "\tPDPEPIL\n");
#endif
#ifdef TARGET_LE
  fprintf(file, "\tEDCEPIL\n");
  fprintf(file, "\tDROP\t%i\n", BASE_REGNUM);
#endif
#else /* !TARGET_MACROS */
#ifdef TARGET_LE
  fprintf(file, "\tL\t13,4(,13)\n");
  fprintf(file, "\tL\t14,12(,13)\n");
  fprintf(file, "\tLM\t2,12,28(13)\n");
  fprintf(file, "\tBALR\t1,14\n");
  fprintf(file, "\tDROP\t%i\n", BASE_REGNUM);
  fprintf(file, "\tDC\tA(");
  assemble_name(file, mvs_function_name);
  fprintf(file, ")\n");
#endif
#endif /* TARGET_MACROS */

  fprintf(file, "* Function %s literal pool\n", CURRFUNC);
  fprintf(file, "\tDS\t0F\n");
  fprintf(file, "\tLTORG\n");
  fprintf(file, "* Function %s page table\n", CURRFUNC);
  fprintf(file, "\tDS\t0F\n");
  fprintf(file, "@@PGT%i\tEQU\t*\n", function_base_page);

  mvs_free_label_list();
  for (int i = function_base_page; i < mvs_page_num; i++)
    fprintf(file, "\tDC\tA(@@PG%i)\n", i);
  mvs_need_entry = 0;
}

#if !defined(TARGET_UDOS) && defined(PUREISO) && !defined(NO_DETAB)

#undef fputs
#undef fprintf
#undef vfprintf
#undef fwrite
#undef fputc

int t_fputs(const char *str, FILE *file)
{
  t_fprintf(file, "%s", str);
  if (ferror(file))
    return (EOF);
  else
    return (0);
}

size_t
t_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *file)
{
  size_t tot;

  tot = size * nmemb;
  t_fprintf(file, "%.*s", tot, ptr);
  return (nmemb);
}

int t_fprintf(FILE *file, const char *format, ...)
{
  va_list arg;
  int ret;

  va_start(arg, format);
  ret = t_vfprintf(file, format, arg);
  va_end(arg);
  return (ret);
}

static int ocnt = 0;
static char obuf[MAX_LEN_OUT];

int t_fputc(int c, FILE *file)
{
  if (c == '\t')
  {
    if (ocnt < 9)
    {
      for (; ocnt < 9; ocnt++)
      {
        obuf[ocnt] = ' ';
      }
    }
    else if (ocnt < 15)
    {
      for (; ocnt < 15; ocnt++)
      {
        obuf[ocnt] = ' ';
      }
    }
    else
    {
      obuf[ocnt] = ' ';
      ocnt++;
    }
  }
  else if (c == '\n')
  {
    t_fprintf(file, "%c", c);
  }
  else
  {
    obuf[ocnt] = c;
    ocnt++;
  }
  return (c);
}

int t_vfprintf(FILE *file, const char *format, va_list arg)
{
  char buf[MAX_LEN_OUT];
  int icnt;

  vsprintf(buf, format, arg);
  icnt = 0;
  while (buf[icnt] != '\0')
  {
    if (buf[icnt] == '\t')
    {
      if (ocnt < 9)
      {
        for (; ocnt < 9; ocnt++)
        {
          obuf[ocnt] = ' ';
        }
      }
      else if (ocnt < 15)
      {
        for (; ocnt < 15; ocnt++)
        {
          obuf[ocnt] = ' ';
        }
      }
      else
      {
        obuf[ocnt] = ' ';
        ocnt++;
      }
    }
    else
    {
      obuf[ocnt] = buf[icnt];
      ocnt++;
      if (buf[icnt] == '\n')
      {
        fwrite(obuf, ocnt, 1, file);
        ocnt = 0;
      }
    }
    icnt++;
  }
  return (icnt);
}

#endif

void i370_register_pragmas(void)
{
#if 0
  cpp_register_pragma (PFILE, 0, "map", i370_pr_map);
  cpp_register_pragma (PFILE, 0, "nomargins", i370_pr_skipit);
  cpp_register_pragma (PFILE, 0, "nosequence", i370_pr_skipit);
  cpp_register_pragma (PFILE, 0, "checkout", i370_pr_checkout);
  cpp_register_pragma (PFILE, 0, "linkage", i370_pr_linkage);
#endif
}

/* Return the size in bytes of a function argument of
   type TYPE and/or mode MODE.  At least one of TYPE or
   MODE must be specified.  */

static int
i370_function_arg_size(machine_mode mode, const_tree type)
{
  if (type)
    return int_size_in_bytes(type);

  /* If we have neither type nor mode, abort */
  gcc_unreachable();
}

/* Return true if a variable of TYPE should be passed as single value
   with type CODE. If STRICT_SIZE_CHECK_P is true the sizes of the
   record type and the field type must match.

   The ABI says that record types with a single member are treated
   just like that member would be.  This function is a helper to
   detect such cases.  The function also produces the proper
   diagnostics for cases where the outcome might be different
   depending on the GCC version.  */
static bool
i370_single_field_struct_p(enum tree_code code, const_tree type,
                           bool strict_size_check_p)
{
  int empty_base_seen = 0;
  bool zero_width_bf_skipped_p = false;
  const_tree orig_type = type;

  while (TREE_CODE(type) == RECORD_TYPE)
  {
    tree field, single_type = NULL_TREE;

    for (field = TYPE_FIELDS(type); field; field = DECL_CHAIN(field))
    {
      if (TREE_CODE(field) != FIELD_DECL)
        continue;

      if (DECL_FIELD_ABI_IGNORED(field))
      {
        if (lookup_attribute("no_unique_address",
                             DECL_ATTRIBUTES(field)))
          empty_base_seen |= 2;
        else
          empty_base_seen |= 1;
        continue;
      }

      if (DECL_FIELD_CXX_ZERO_WIDTH_BIT_FIELD(field))
      {
        zero_width_bf_skipped_p = true;
        continue;
      }

      if (single_type == NULL_TREE)
        single_type = TREE_TYPE(field);
      else
        return false;
    }

    if (single_type == NULL_TREE)
      return false;

    /* Reaching this point we have a struct with a single member and
 zero or more zero-sized bit-fields which have been skipped in the
 past.  */

    /* If ZERO_WIDTH_BF_SKIPPED_P then the struct will not be accepted.  In case
 we are not supposed to emit a warning exit early.  */
    if (zero_width_bf_skipped_p && !warn_psabi)
      return false;

    /* If the field declaration adds extra bytes due to padding this
 is not accepted with STRICT_SIZE_CHECK_P.  */
    if (strict_size_check_p && (int_size_in_bytes(single_type) <= 0 || int_size_in_bytes(single_type) != int_size_in_bytes(type)))
      return false;

    type = single_type;
  }

  if (TREE_CODE(type) != code)
    return false;

  if (warn_psabi)
  {
    unsigned uid = TYPE_UID(TYPE_MAIN_VARIANT(orig_type));

    if (empty_base_seen)
    {
      static unsigned last_reported_type_uid_empty_base;
      if (uid != last_reported_type_uid_empty_base)
      {
        last_reported_type_uid_empty_base = uid;
        const char *url = CHANGES_ROOT_URL "gcc-10/changes.html#empty_base";
        if (empty_base_seen & 1)
          inform(input_location,
                 "parameter passing for argument of type %qT when C++17 "
                 "is enabled changed to match C++14 %{in GCC 10.1%}",
                 orig_type, url);
        else
          inform(input_location,
                 "parameter passing for argument of type %qT with "
                 "%<[[no_unique_address]]%> members changed "
                 "%{in GCC 10.1%}",
                 orig_type, url);
      }
    }

    /* For C++ older GCCs ignored zero width bitfields and therefore
 passed structs more often as single values than GCC 12 does.
 So diagnostics are only required in cases where we do NOT
 accept the struct to be passed as single value.  */
    if (zero_width_bf_skipped_p)
    {
      static unsigned last_reported_type_uid_zero_width;
      if (uid != last_reported_type_uid_zero_width)
      {
        last_reported_type_uid_zero_width = uid;
        inform(input_location,
               "parameter passing for argument of type %qT with "
               "zero-width bit fields members changed in GCC 12",
               orig_type);
      }
    }
  }

  return !zero_width_bf_skipped_p;
}

/* Return true if a function argument of type TYPE and mode MODE
   is to be passed in a vector register, if available.  */

static bool
i370_function_arg_vector(machine_mode mode, const_tree type)
{
  if (i370_function_arg_size(mode, type) > 16)
    return false;

  /* No type info available for some library calls ...  */
  if (!type)
    return VECTOR_MODE_P(mode);

  if (!i370_single_field_struct_p(VECTOR_TYPE, type, true))
    return false;

  return true;
}

/* Return true if a function argument of type TYPE and mode MODE
   is to be passed in a floating-point register, if available.  */

static bool
i370_function_arg_float(machine_mode mode, const_tree type)
{
  if (i370_function_arg_size(mode, type) > 8)
    return false;

  /* Soft-float changes the ABI: no floating-point registers are used.  */
  if (TARGET_SOFT_FLOAT)
    return false;

  /* No type info available for some library calls ...  */
  if (!type)
    return mode == SFmode || mode == DFmode || mode == SDmode || mode == DDmode;

  if (!i370_single_field_struct_p(REAL_TYPE, type, false))
    return false;

  return true;
}

/* Return true if a function argument of type TYPE and mode MODE
   is to be passed in an integer register, or a pair of integer
   registers, if available.  */

static bool
i370_function_arg_integer(machine_mode mode, const_tree type)
{
  int size = i370_function_arg_size(mode, type);
  if (size > 8)
    return false;

  /* No type info available for some library calls ...  */
  if (!type)
    return GET_MODE_CLASS(mode) == MODE_INT || (TARGET_SOFT_FLOAT && SCALAR_FLOAT_MODE_P(mode));

  /* We accept small integral (and similar) types.  */
  if (INTEGRAL_TYPE_P(type) || POINTER_TYPE_P(type) || TREE_CODE(type) == NULLPTR_TYPE || TREE_CODE(type) == OFFSET_TYPE || (TARGET_SOFT_FLOAT && TREE_CODE(type) == REAL_TYPE))
    return true;

  /* We also accept structs of size 1, 2, 4, 8 that are not
     passed in floating-point registers.  */
  if (AGGREGATE_TYPE_P(type) && exact_log2(size) >= 0 && !i370_function_arg_float(mode, type))
    return true;

  return false;
}

/* Return 1 if a function argument ARG is to be passed by reference.
   The ABI specifies that only structures of size 1, 2, 4, or 8 bytes
   are passed by value, all other structures (and complex numbers) are
   passed by reference.  */

static bool
i370_pass_by_reference(cumulative_args_t, const function_arg_info &arg)
{
  int size = i370_function_arg_size(arg.mode, arg.type);

  if (i370_function_arg_vector(arg.mode, arg.type))
    return false;

  if (size > 8)
    return true;

  if (tree type = arg.type)
  {
    if (AGGREGATE_TYPE_P(type) && exact_log2(size) < 0)
      return true;

    if (TREE_CODE(type) == COMPLEX_TYPE || TREE_CODE(type) == VECTOR_TYPE)
      return true;
  }

  return false;
}

/* Update the data in CUM to advance over argument ARG.  */

static void
i370_function_arg_advance(cumulative_args_t cum_v,
                          const function_arg_info &arg)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args(cum_v);
  int size = i370_function_arg_size(arg.mode, arg.type);
  cum += ((size + UNITS_PER_LONG - 1) / UNITS_PER_LONG);
}

/* Set the vector ABI marker if TYPE is subject to the vector ABI
   switch.  The vector ABI affects only vector data types.  There are
   two aspects of the vector ABI relevant here:

   1. vectors >= 16 bytes have an alignment of 8 bytes with the new
   ABI and natural alignment with the old.

   2. vector <= 16 bytes are passed in VRs or by value on the stack
   with the new ABI but by reference on the stack with the old.

   If ARG_P is true TYPE is used for a function argument or return
   value.  The ABI marker then is set for all vector data types.  If
   ARG_P is false only type 1 vectors are being checked.  */

/* Define where to put the arguments to a function.
   Value is zero to push the argument on the stack,
   or a hard register in which to store the argument.

   CUM is a variable of type CUMULATIVE_ARGS which gives info about
    the preceding args and about the function being called.
   ARG is a description of the argument.

   On S/390, we use general purpose registers 2 through 6 to
   pass integer, pointer, and certain structure arguments, and
   floating point registers 0 and 2 (0, 2, 4, and 6 on 64-bit)
   to pass floating point arguments.  All remaining arguments
   are pushed to the stack.  */

#define GP_ARG_NUM_REG 8
#define FP_ARG_NUM_REG 4

static rtx
i370_function_arg(cumulative_args_t cum_v, const function_arg_info &arg)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args(cum_v);

  if (i370_function_arg_float(arg.mode, arg.type) || i370_function_arg_integer(arg.mode, arg.type))
  {
    return NULL_RTX;
  }

  /* After the real arguments, expand_call calls us once again with an
     end marker.  Whatever we return here is passed as operand 2 to the
     call expanders.

     We don't need this feature ...  */
  else if (arg.end_marker_p())
    return const0_rtx;

  gcc_unreachable();
}

/* Implement TARGET_FUNCTION_ARG_BOUNDARY.  Vector arguments are
   left-justified when placed on the stack during parameter passing.  */

static pad_direction
i370_function_arg_padding(machine_mode mode, const_tree type)
{
  return PAD_UPWARD;
}

/* Return true if return values of type TYPE should be returned
   in a memory buffer whose address is passed by the caller as
   hidden first argument.  */

static bool
i370_return_in_memory(const_tree type, const_tree fundecl ATTRIBUTE_UNUSED)
{
  /* We accept small integral (and similar) types.  */
  if (INTEGRAL_TYPE_P(type) || POINTER_TYPE_P(type) || TREE_CODE(type) == OFFSET_TYPE || TREE_CODE(type) == REAL_TYPE)
    return int_size_in_bytes(type) > 8;

  /* vector types which fit into a VR.  */
  if (TARGET_VX_ABI && VECTOR_TYPE_P(type) && int_size_in_bytes(type) <= 16)
    return false;

  /* Aggregates and similar constructs are always returned
     in memory.  */
  if (AGGREGATE_TYPE_P(type) || TREE_CODE(type) == COMPLEX_TYPE || VECTOR_TYPE_P(type))
    return true;

  /* ??? We get called on all sorts of random stuff from
     aggregate_value_p.  We can't abort, but it's not clear
     what's safe to return.  Pretend it's a struct I guess.  */
  return true;
}

void i370_asm_output_ident_directive(const char *ident_str)
{
  fprintf(asm_out_file, "* Ident: %s\n", ident_str);
}

void i370_stabs_asm_out_constructor(rtx symbol, int priority ATTRIBUTE_UNUSED)
{
  fprintf(asm_out_file, "* CTOR,Prio=%i", priority);
  assemble_name(asm_out_file, XSTR(symbol, 0));
  fputc('\n', asm_out_file);
}

void i370_stabs_asm_out_destructor(rtx symbol, int priority ATTRIBUTE_UNUSED)
{
  fprintf(asm_out_file, "* DTOR,Prio=%i", priority);
  assemble_name(asm_out_file, XSTR(symbol, 0));
  fputc('\n', asm_out_file);
}

/* Checks whether the given CALL_EXPR would use a caller
   saved register.  This is used to decide whether sibling call
   optimization could be performed on the respective function
   call.  */

static bool
i370_call_saved_register_used(tree call_expr)
{
  CUMULATIVE_ARGS cum_v;
  INIT_CUMULATIVE_ARGS(cum_v, NULL, NULL, 0, 0);
  cumulative_args_t cum = pack_cumulative_args(&cum_v);
  for (int i = 0; i < call_expr_nargs(call_expr); i++)
  {
    tree parameter = CALL_EXPR_ARG(call_expr, i);
    gcc_assert(parameter);

    /* For an undeclared variable passed as parameter we will get
     an ERROR_MARK node here.  */
    if (TREE_CODE(parameter) == ERROR_MARK)
      return true;

    /* We assume that in the target function all parameters are
     named.  This only has an impact on vector argument register
     usage none of which is call-saved.  */
    function_arg_info arg(TREE_TYPE(parameter), /*named=*/true);
    apply_pass_by_reference_rules(&cum_v, arg);

    rtx parm_rtx = i370_function_arg(cum, arg);

    i370_function_arg_advance(cum, arg);

    if (!parm_rtx)
      continue;

    if (REG_P(parm_rtx))
    {
      for (int reg = 0; reg < REG_NREGS(parm_rtx); reg++)
        if (!call_used_or_fixed_reg_p(reg + REGNO(parm_rtx)))
          return true;
    }

    if (GET_CODE(parm_rtx) == PARALLEL)
    {
      for (int i = 0; i < XVECLEN(parm_rtx, 0); i++)
      {
        rtx r = XEXP(XVECEXP(parm_rtx, 0, i), 0);
        gcc_assert(REG_P(r));
        for (int reg = 0; reg < REG_NREGS(r); reg++)
          if (!call_used_or_fixed_reg_p(reg + REGNO(r)))
            return true;
      }
    }
  }
  return false;
}

/* Return true if the given call expression can be
   turned into a sibling call.
   DECL holds the declaration of the function to be called whereas
   EXP is the call expression itself.  */

static bool
i370_function_ok_for_sibcall(tree decl, tree exp)
{
  printf("i370_function_ok_for_sibcall()\n");
  /* The 31 bit PLT code uses register 12 (GOT pointer - caller saved)
     which would have to be restored before the sibcall.  */
  if (flag_pic && decl && !targetm.binds_local_p(decl))
    return false;

  /* The thunks for indirect branches require r1 if no exrl is
     available.  r1 might not be available when doing a sibling
     call.  */
  if (!decl)
    return false;

  /* Register 6 on s390 is available as an argument register but unfortunately
     "caller saved". This makes functions needing this register for arguments
     not suitable for sibcalls.  */
  return !i370_call_saved_register_used(exp);
}

/* Define where to return a (scalar) value of type RET_TYPE.
   If RET_TYPE is null, define where to return a (scalar)
   value of mode MODE from a libcall.  */

static rtx
i370_function_and_libcall_value(machine_mode mode,
                                const_tree ret_type,
                                const_tree fntype_or_decl,
                                bool outgoing ATTRIBUTE_UNUSED)
{
  /* For normal functions perform the promotion as
     promote_function_mode would do.  */
  if (ret_type)
  {
    int unsignedp = TYPE_UNSIGNED(ret_type);
    mode = promote_function_mode(ret_type, mode, &unsignedp,
                                 fntype_or_decl, 1);
  }

  gcc_assert(GET_MODE_CLASS(mode) == MODE_INT || SCALAR_FLOAT_MODE_P(mode));
  gcc_assert(GET_MODE_SIZE(mode) <= 8);

  if (GET_MODE_SIZE(mode) <= UNITS_PER_LONG || UNITS_PER_LONG == UNITS_PER_WORD)
  {
    return gen_rtx_REG(mode, 2);
  }
  else if (GET_MODE_SIZE(mode) == 2 * UNITS_PER_LONG)
  {
    /* This case is triggered when returning a 64 bit value with
     -m31 -mzarch.  Although the value would fit into a single
     register it has to be forced into a 32 bit register pair in
     order to match the ABI.  */
    rtvec p = rtvec_alloc(2);
    RTVEC_ELT(p, 0) = gen_rtx_EXPR_LIST(SImode, gen_rtx_REG(SImode, 2), const0_rtx);
    RTVEC_ELT(p, 1) = gen_rtx_EXPR_LIST(SImode, gen_rtx_REG(SImode, 3), GEN_INT(4));
    return gen_rtx_PARALLEL(mode, p);
  }

  gcc_unreachable();
}

/* Define where to return a scalar libcall return value of mode
   MODE.  */
static rtx
i370_libcall_value(machine_mode mode, const_rtx fun ATTRIBUTE_UNUSED)
{
  return i370_function_and_libcall_value(mode, NULL_TREE,
                                         NULL_TREE, true);
}

/* Define where to return a scalar return value of type RET_TYPE.  */
static rtx
i370_function_value(const_tree ret_type, const_tree fn_decl_or_type,
                    bool outgoing)
{
  return i370_function_and_libcall_value(TYPE_MODE(ret_type), ret_type,
                                         fn_decl_or_type, outgoing);
}

/* Define platform dependent macros.  */
void i370_cpu_cpp_builtins(cpp_reader *pfile)
{
  cpp_define(pfile, "__i370__");
  /* MVS-TODO: Define __MVS__, __VSE__, __CMS__, __MUSIC__, __PDOS__, __UDOS__, etc */
  struct cl_target_option opts;
  cl_target_option_save(&opts, &global_options, &global_options_set);
}

/* Convert a float to a printable form.  */
static char *
mvs_make_float(const REAL_VALUE_TYPE* r)
{
  static char buf[50];
  long int f;
  /* MVS-TODO: Fix floats */
  REAL_VALUE_TO_TARGET_LONG_DOUBLE(*r, &f);
  snprintf(buf, sizeof buf, "%.4lf", (long double)f);
  return (buf);
}

void i370_print_operand(FILE *f, rtx xv, int code)
{
  switch (GET_CODE(xv))
  {
    static char curreg[4];
  case REG:
    if (code == 'N')
      strcpy(curreg, reg_names[REGNO(xv) + 1]);
    else
      strcpy(curreg, reg_names[REGNO(xv)]);
    fprintf(f, "%s", curreg);
    break;
  case MEM:
  {
    rtx addr = XEXP(xv, 0);
    if (code == 'O')
    {
      if (GET_CODE(addr) == PLUS)
        fprintf(f, "%i", INTVAL(XEXP(addr, 1)));
      else
        fprintf(f, "0");
    }
    else if (code == 'R')
    {
      if (GET_CODE(addr) == PLUS)
        fprintf(f, "%s", reg_names[REGNO(XEXP(addr, 0))]);
      else
        fprintf(f, "%s", reg_names[REGNO(addr)]);
    }
    else
      output_address(GET_MODE(xv), XEXP(xv, 0));
  }
  break;
  case SYMBOL_REF:
  case LABEL_REF:
    mvs_page_lit += 4;
    if (SYMBOL_REF_FLAG(xv))
    {
      fprintf(f, "=V(");
      i370_asm_label_ref(f, XSTR(xv, 0));
      fprintf(f, ")");
      mvs_mark_alias(XSTR(xv, 0));
    }
    else
    {
      fprintf(f, "=A(");
      i370_asm_label_ref(f, XSTR(xv, 0));
      fprintf(f, ")");
    }
    break;
  case CONST_INT:
    if (code == 'B')
      fprintf(f, "%i", INTVAL(xv) & 0xff);
    else if (code == 'X')
      fprintf(f, "%02X", INTVAL(xv) & 0xff);
    else if (code == 'h')
      fprintf(f, "%i", (INTVAL(xv) << 16) >> 16);
    else if (code == 'H')
    {
      mvs_page_lit += 2;
      fprintf(f, "=H'%i'", (INTVAL(xv) << 16) >> 16);
    }
    else if (code == 'K')
    {
      /* auto sign-extension of signed 16-bit to signed 32-bit */
      mvs_page_lit += 4;
      fprintf(f, "=F'%i'", (INTVAL(xv) << 16) >> 16);
    }
    else if (code == 'W')
    {
      /* hand-built sign-extension of signed 32-bit to 64-bit */
      mvs_page_lit += 8;
      if (0 <= INTVAL(xv))
      {
        fprintf(f, "=XL8'00000000");
      }
      else
      {
        fprintf(f, "=XL8'FFFFFFFF");
      }
      fprintf(f, "%08X'", INTVAL(xv));
    }
    else
    {
      mvs_page_lit += 4;
      fprintf(f, "=F'%i'", INTVAL(xv));
    }
    break;
  case CONST_DOUBLE:
    if (GET_MODE(xv) == DImode)
    {
      if (code == 'M')
      {
        mvs_page_lit += 4;
        fprintf(f, "=XL4'%08X'", CONST_DOUBLE_LOW(xv));
      }
      else if (code == 'L')
      {
        mvs_page_lit += 4;
        fprintf(f, "=XL4'%08X'", CONST_DOUBLE_HIGH(xv));
      }
      else
      {
        mvs_page_lit += 8;
        fprintf(f, "=XL8'%08X%08X'", CONST_DOUBLE_LOW(xv),
                CONST_DOUBLE_HIGH(xv));
      }
    }
    else
    {
      if (GET_MODE(xv) == SFmode)
      {
        const REAL_VALUE_TYPE* rval = CONST_DOUBLE_REAL_VALUE(xv);
        mvs_page_lit += 4;
        fprintf(f, "=E'%s'", mvs_make_float(rval));
      }
      else if (GET_MODE(xv) == DFmode)
      {
        const REAL_VALUE_TYPE* rval = CONST_DOUBLE_REAL_VALUE(xv);
        mvs_page_lit += 8;
        fprintf(f, "=D'%s'", mvs_make_float(rval));
      }
      else
      {
        mvs_page_lit += 8;
        fprintf(f, "=XL8'%08X%08X'",
                CONST_DOUBLE_HIGH(xv), CONST_DOUBLE_LOW(xv));
      }
    }
    break;
  case CONST:
    if (GET_CODE(XEXP(xv, 0)) == PLUS && GET_CODE(XEXP(XEXP(xv, 0), 0)) == SYMBOL_REF)
    {
      mvs_page_lit += 4;
      if (SYMBOL_REF_FLAG(XEXP(XEXP(xv, 0), 0)))
      {
        int xx = INTVAL(XEXP(XEXP(xv, 0), 1));
        fprintf(f, "=V(");
        ASM_OUTPUT_LABELREF(f,
                            XSTR(XEXP(XEXP(xv, 0), 0), 0));
        if ((unsigned)xx < 4096)
          fprintf(f, ")\n\tLA\t%s,%i(0,%s)", curreg, xx, curreg);
        else
          fprintf(f, ")\n\tA\t%s,=F'%i'", curreg, xx);
        mvs_mark_alias(XSTR(XEXP(XEXP(xv, 0), 0), 0));
      }
      else
      {
        fprintf(f, "=A(");
        i370_asm_label_ref(f, XSTR(xv, 0));
        fprintf(f, ")");
      }
    }
    else
    {
      mvs_page_lit += 4;
      fprintf(f, "=F'");
      output_addr_const(f, xv);
      fprintf(f, "'");
    }
    break;
  default:
    gcc_unreachable();
  }
}

#undef TARGET_LIBCALL_VALUE
#define TARGET_LIBCALL_VALUE i370_libcall_value
#undef TARGET_PASS_BY_REFERENCE
#define TARGET_PASS_BY_REFERENCE i370_pass_by_reference
#undef TARGET_FUNCTION_OK_FOR_SIBCALL
#define TARGET_FUNCTION_OK_FOR_SIBCALL i370_function_ok_for_sibcall
#undef TARGET_FUNCTION_ARG
#define TARGET_FUNCTION_ARG i370_function_arg
#undef TARGET_FUNCTION_ARG_ADVANCE
#define TARGET_FUNCTION_ARG_ADVANCE i370_function_arg_advance
#undef TARGET_FUNCTION_ARG_PADDING
#define TARGET_FUNCTION_ARG_PADDING i370_function_arg_padding
#undef TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE i370_function_value
#undef TARGET_ASM_OUTPUT_IDENT
#define TARGET_ASM_OUTPUT_IDENT i370_asm_output_ident_directive
#undef TARGET_ASM_CONSTRUCTOR
#define TARGET_ASM_CONSTRUCTOR i370_stabs_asm_out_constructor
#undef TARGET_ASM_DESTRUCTOR
#define TARGET_ASM_DESTRUCTOR i370_stabs_asm_out_destructor
#undef TARGET_ASM_FUNCTION_PROLOGUE
#define TARGET_ASM_FUNCTION_PROLOGUE i370_output_function_prologue
#undef TARGET_ASM_FUNCTION_EPILOGUE
#define TARGET_ASM_FUNCTION_EPILOGUE i370_output_function_epilogue

/* This file should be included last.  */
#include "target-def.h"
struct gcc_target targetm = TARGET_INITIALIZER;
