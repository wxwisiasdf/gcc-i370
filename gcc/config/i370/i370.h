/* Definitions of target machine for GNU compiler.  System/370 version.
   Copyright (C) 1989, 1993, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002
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

#ifndef GCC_I370_H
#define GCC_I370_H

#include "mvs.h"

/* Run-time compilation parameters selecting different hardware subsets.  */

/* The sizes of the code and literals on the current page.  */

extern int mvs_page_code, mvs_page_lit;

/* The length of code used by case labels */

extern int mvs_case_code;

/* Specify that we need to be an entry point.  */

extern int mvs_need_entry;
extern int mvs_gotmain;
extern int mvs_need_to_globalize;

/* The current page number and the base page number for the function.  */

extern int mvs_page_num, function_base_page;

/* The name of the current function.  */

extern char *mvs_function_name;

/* The length of the function name malloc'd area.  */

extern int mvs_function_name_length;

/* The desired CSECT name */

extern char *mvs_csect_name;

/* The source file module.  */

extern char *mvs_module;

#define PREFERRED_DEBUGGING_TYPE 0

/* Compile using char instructions (mvc, nc, oc, xc).  On 4341 use this since
   these are more than twice as fast as load-op-store.
   On 3090 don't use this since load-op-store is much faster.
   On Hercules it seems that the char instructions make a
   module slightly faster.  */

#define TARGET_CHAR_INSTRUCTIONS (target_flags & 1)

/* Compile experimental position independent code */
#define TARGET_PIC (target_flags & 2)
extern int i370_enable_pic;

/* Default target switches */
/* This appears to be what switches
   target char instructions on by default */

#define TARGET_DEFAULT 1

/* Macro to define tables used to set the flags.  This is a list in braces
   of pairs in braces, each pair being { "NAME", VALUE }
   where VALUE is the bits to set or minus the bits to clear.
   An empty string NAME is used to identify the default VALUE.  */

/*
#define TARGET_SWITCHES							\
{ { "char-instructions", 1, N_("Generate char instructions")},            \
  { "no-char-instructions", -1, N_("Do not generate char instructions")}, \
  { "pickax", 2, "Experimental i370 PIC"}, \
  { "no-pickax", -2, "Disable experimental i370 PIC"}, \
  { "", TARGET_DEFAULT, 0} }
*/

/*
#define TARGET_OPTIONS \
{ { "csect=", (const char **)&mvs_csect_name, \
    N_("Set CSECT name")},     \
  SUBTARGET_OPTIONS    \
}
*/

#define SUBTARGET_OPTIONS

#define REAL_ARITHMETIC

extern void i370_override_options(void);
extern void mvs_mark_alias(const char *);
/*
#define OVERRIDE_OPTIONS i370_override_options()
*/

/* To use IBM supplied macro function prologue and epilogue, define the
   following to 1.  Should only be needed if IBM changes the definition
   of their prologue and epilogue.  */

#define MACROPROLOGUE 0
#define MACROEPILOGUE 0

/* Target machine storage layout */

/* Define this if most significant bit is lowest numbered in instructions
   that operate on numbered bit-fields.  */

#define BITS_BIG_ENDIAN 1

/* Define this if most significant byte of a word is the lowest numbered.  */

#define BYTES_BIG_ENDIAN 1

/* Define this if MS word of a multiword is the lowest numbered.  */

#define WORDS_BIG_ENDIAN 1

/* Width in bits of a "word", which is the contents of a machine register.  */

#define BITS_PER_WORD 32

/* Width of a word, in units (bytes).  */

#define UNITS_PER_WORD 4

/* Width of a word, in units (bytes).  */

#define UNITS_PER_LONG 4

/* Width in bits of a pointer.  See also the macro `Pmode' defined below.  */

#define POINTER_SIZE 32

/* Allocation boundary (in *bits*) for storing pointers in memory.  */

#define POINTER_BOUNDARY 32

/* Allocation boundary (in *bits*) for storing arguments in argument list.  */

#define PARM_BOUNDARY 32

/* Boundary (in *bits*) on which stack pointer should be aligned.  */

#define STACK_BOUNDARY 64

/* Allocation boundary (in *bits*) for the code of a function.  */

#define FUNCTION_BOUNDARY 32

/* There is no point aligning anything to a rounder boundary than this.  */

#define BIGGEST_ALIGNMENT 64

/* Alignment of field after `int : 0' in a structure.  */

#define EMPTY_FIELD_BOUNDARY 8

/* Define this if move instructions will actually fail to work when given
   unaligned data.  */

#define STRICT_ALIGNMENT 0

/* Define target floating point format.  */

#define TARGET_FLOAT_FORMAT IBM_FLOAT_FORMAT

/* Define character mapping for cross-compiling.  */
/* but only define it if really needed, since otherwise it will break builds */

#ifdef TARGET_EBCDIC
#ifdef HOST_EBCDIC
#define MAP_OUTCHAR(c) (c)
#define MAP_INCHAR(c) (c)
#else
#define MAP_OUTCHAR(c) (ASCTOEBC(c))
#define MAP_INCHAR(c) (EBCTOASC(c))
#endif
#else
#ifdef HOST_EBCDIC
#define MAP_OUTCHAR(c) ((char)EBCTOASC(c))
#define MAP_INCHAR(c) (ASCTOEBC(c))
#endif
#endif

#ifndef TARGET_SOFT_FLOAT
#define TARGET_SOFT_FLOAT 1
#endif

#ifdef TARGET_HLASM
#include "i370-protos.h"
/* HLASM requires #pragma map.  */
#define REGISTER_TARGET_PRAGMAS() \
  do                              \
  {                               \
    i370_register_pragmas();      \
  } while (0)
#endif /* TARGET_HLASM */

/* Define maximum length of page minus page escape overhead.  */

#define MAX_MVS_PAGE_LENGTH 4068

/*
#define PREDICATE_CODES \
  {"r_or_s_operand", { REG, SUBREG, MEM }}, \
  {"s_operand", { MEM }},
*/

/* Conservative page size when considering literals and overhead. */

#define MVS_PAGE_CONSERVATIVE 4060

/* Define special register allocation order desired. */

#define FIXED_REGISTERS \
  {                     \
    0, 0, 0, 0,         \
        0, 0, 0, 0,     \
        0, 0, 0, 0,     \
        0, 1, 1, 1,     \
        0, 0, 0, 0,     \
        0, 0, 0, 0,     \
        0, 0, 0, 0,     \
        0, 0, 0, 0,     \
        1, 1, 1, 1,     \
        1, 1,           \
        0, 0, 0, 0,     \
        0, 0, 0, 0,     \
        0, 0, 0, 0,     \
        0, 0, 0, 0      \
  }

#define CALL_REALLY_USED_REGISTERS              \
  {                                             \
    1, 1, 1, 1, /* r0 - r15 */                  \
        1, 1, 0, 0,                             \
        0, 0, 0, 0,                             \
        0, 0, 0, 0,                             \
        1, 1, 1, 1, /* f0 (16) - f15 (31) */    \
        1, 1, 1, 1,                             \
        1, 1, 1, 1,                             \
        1, 1, 1, 1,                             \
        1, 1, 1, 1, /* arg, cc, fp, ret addr */ \
        0, 0,       /* a0 (36), a1 (37) */      \
        1, 1, 1, 1, /* v16 (38) - v23 (45) */   \
        1, 1, 1, 1,                             \
        1, 1, 1, 1, /* v24 (46) - v31 (53) */   \
        1, 1, 1, 1                              \
  }

/* Preferred register allocation order.  */
#define REG_ALLOC_ORDER                                                 \
  {                                                                     \
    1, 2, 3, 4, 5, 0, 12, 11, 10, 9, 8, 7, 6, 14, 13,                   \
        16, 17, 18, 19, 20, 21, 22, 23,                                 \
        24, 25, 26, 27, 28, 29, 30, 31,                                 \
        38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, \
        15, 32, 33, 34, 35, 36, 37                                      \
  }

/* Standard register usage.  */

/* Number of actual hardware registers.  The hardware registers are
   assigned numbers for the compiler from 0 to just below
   FIRST_PSEUDO_REGISTER.
   All registers that the compiler knows about must be given numbers,
   even those that are not normally considered general registers.
   For the 370, we give the data registers numbers 0-15,
   and the floating point registers numbers 16-19.  */

#define FIRST_PSEUDO_REGISTER 54

/* Define the classes of registers for register constraints in the
   machine description.  Also define ranges of constants.

   One of the classes must always be named ALL_REGS and include all hard regs.
   If there is more than one class, another class must be named NO_REGS
   and contain no registers.

   The name GENERAL_REGS must be the name of a class (or an alias for
   another name such as ALL_REGS).  This is the class of registers
   that is allowed by "g" or "r" in a register constraint.
   Also, registers outside this class are allocated only when
   instructions express preferences for them.

   The classes must be numbered in nondecreasing order; that is,
   a larger-numbered class must never be contained completely
   in a smaller-numbered class.

   For any two classes, it is very desirable that there be another
   class that represents their union.  */

enum reg_class
{
  NO_REGS,
  ADDR_REGS,
  DATA_REGS,
  FP_REGS,
  ALL_REGS,
  LIM_REG_CLASSES
};

#define GENERAL_REGS DATA_REGS
#define N_REG_CLASSES (int)LIM_REG_CLASSES

/* Give names of register classes as strings for dump file.  */

#define REG_CLASS_NAMES                                        \
  {                                                            \
    "NO_REGS", "ADDR_REGS", "DATA_REGS", "FP_REGS", "ALL_REGS" \
  }

/* Define which registers fit in which classes.  This is an initializer for
   a vector of HARD_REG_SET of length N_REG_CLASSES.  */

#define REG_CLASS_CONTENTS                            \
  {                                                   \
    {0}, {0x0fffe}, {0x0ffff}, {0xf0000}, { 0xfffff } \
  }

/* The same information, inverted:
   Return the class number of the smallest class containing
   reg number REGNO.  This could be a conditional expression
   or could index an array.  */

#define REGNO_REG_CLASS(REGNO)                        \
  ((REGNO) >= 16 ? FP_REGS : (REGNO) != 0 ? ADDR_REGS \
                                          : DATA_REGS)

/* The letters I, J, K, L and M in a register constraint string can be used
   to stand for particular ranges of immediate operands.
   This macro defines what the ranges are.
   C is the letter, and VALUE is a constant value.
   Return 1 if VALUE is in the range specified by C.  */

#define CONST_OK_FOR_LETTER_P_NO_POISON(VALUE, C)					\
  ((C) == 'I' ? (unsigned) (VALUE) < 256 :				\
   (C) == 'J' ? (unsigned) (VALUE) < 4096 :				\
   (C) == 'K' ? (VALUE) >= -32768 && (VALUE) < 32768 : 0)

/* The class value for index registers, and the one for base regs.  */

#define INDEX_REG_CLASS ADDR_REGS
#define BASE_REG_CLASS ADDR_REGS

/* Get reg_class from a letter such as appears in the machine description.  */

/*
#define REG_CLASS_FROM_LETTER(C)					\
  ((C) == 'a' ? ADDR_REGS :						\
  ((C) == 'd' ? DATA_REGS :						\
  ((C) == 'f' ? FP_REGS   : NO_REGS)))
*/

/* The letters I, J, K, L and M in a register constraint string can be used
   to stand for particular ranges of immediate operands.
   This macro defines what the ranges are.
   C is the letter, and VALUE is a constant value.
   Return 1 if VALUE is in the range specified by C.  */

#define CONST_CLOBBER_CHECK_OK_FOR_LETTER_P(VALUE, C)                                       \
  ((C) == 'I' ? (unsigned)(VALUE) < 256 : (C) == 'J' ? (unsigned)(VALUE) < 4096             \
                                      : (C) == 'K'   ? (VALUE) >= -32768 && (VALUE) < 32768 \
                                                     : 0)

/* Similar, but for floating constants, and defining letters G and H.
   Here VALUE is the CONST_DOUBLE rtx itself.  */

/*
#define CONST_DOUBLE_OK_FOR_LETTER_P(VALUE, C)  1
*/

/* see recog.c for details */
/*
#define EXTRA_CONSTRAINT(OP,C)						\
   ((C) == 'R' ? r_or_s_operand (OP, GET_MODE(OP)) :			\
    (C) == 'S' ? s_operand (OP, GET_MODE(OP)) :	0)			\
*/

/* Given an rtx X being reloaded into a reg required to be in class CLASS,
   return the class of reg to actually use.  In general this is just CLASS;
   but on some machines in some cases it is preferable to use a more
   restrictive class.

   XXX We reload CONST_INT's into ADDR not DATA regs because on certain
   rare occasions when lots of egisters are spilled, reload() will try
   to put a const int into r0 and then use r0 as an index register.
*/

#define PREFERRED_RELOAD_CLASS(X, CLASS)                                                                                                                              \
  (GET_CODE(X) == CONST_DOUBLE ? FP_REGS : GET_CODE(X) == CONST_INT                                                    ? (reload_in_progress ? ADDR_REGS : DATA_REGS) \
                                       : GET_CODE(X) == LABEL_REF || GET_CODE(X) == SYMBOL_REF || GET_CODE(X) == CONST ? ADDR_REGS                                    \
                                                                                                                       : (CLASS))

/* Return the maximum number of consecutive registers needed to represent
   mode MODE in a register of class CLASS.
   Note that DCmode (complex double) needs two regs.
*/

#define CLASS_MAX_NREGS(CLASS, MODE) \
  ((CLASS) == FP_REGS ? ((GET_MODE_SIZE(MODE) + 2 * UNITS_PER_WORD - 1) / (2 * UNITS_PER_WORD)) : (GET_MODE_SIZE(MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/* Stack layout; function entry, exit and calling.  */

/* Define this if pushing a word on the stack makes the stack pointer a
   smaller address.  */

/* #define STACK_GROWS_DOWNWARD */

/* Define this if the nominal address of the stack frame is at the
   high-address end of the local variables; that is, each additional local
   variable allocated goes at a more negative offset in the frame.  */

/* #define FRAME_GROWS_DOWNWARD */

/* ------------------------------------------------------------------- */
/* ================= */
#ifdef TARGET_HLASM

/* Define offset from stack pointer, to location where a parm can be
   pushed.  */

#if defined(TARGET_DIGNUS) || defined(TARGET_PDPMAC)
#define STACK_POINTER_OFFSET 88
#else
#define STACK_POINTER_OFFSET 148
#endif

#define STACK_FRAME_BASE 88

/* used in i370.md for temp scratch area */
#if defined(TARGET_DIGNUS) || defined(TARGET_PDPMAC)
#define CONVLO "80"
#define CONVHI "84"
#else
#define CONVLO "140"
#define CONVHI "144"
#endif

/* Offset within stack frame to start allocating local variables at.
   If FRAME_GROWS_DOWNWARD, this is the offset to the END of the
   first local allocated.  Otherwise, it is the offset to the BEGINNING
   of the first local allocated.  */

/* Offset of first parameter from the argument pointer register value.  */

#define FIRST_PARM_OFFSET(FNDECL) 0

/* Accumulate the outgoing argument count so we can request the right
   DSA size and determine stack offset.  */

#define ACCUMULATE_OUTGOING_ARGS 1

#endif /* TARGET_HLASM */

/* ================= */
#ifdef TARGET_LINUX
/* Here's the stack layout as currently designed:

   r11 -- top of stack aka stack pointer
   -4(r11) -- last local (stack) variable)
   ...          ...
   88+4*nargs(r13) -- first local (stack) variable.
   ...          ...
   92(r13) -- second incoming (callee) argument
   88(r13) -- first incoming (callee) argument
   84(r13) -- volatile scratch area
   80(r13) -- volatile scratch area
   76(r13) -- not used (frame size)
   72(r13) -- not used
   68(r13) -- saved callers r12
   64(r13) -- saved callers r11
   ...          ...
   28(r13) -- saved callers r2
   24(r13) -- saved callers r1
   20(r13) -- saved callers r0
   16(r13) -- saved callers r15
   12(r13) -- saved callers r14
   8(r13)  -- saved callers r13
   4(r13)  -- not used
   0(r13)  -- code page table pointer
   r13 -- bottom of stack aka frame pointer aka arg pointer

   Note that this bears superficial similarity to the MVS/OE stack layout,
   but in fact it is very very different.  In particular, under MVS/OE
   the roles of r11 and r13 are quite different.

   Note that the use of varargs/stdarg is limited to 512 bytes of
   of arguments.  This is the price that is paid for freeing up a
   register and having a more efficient function return.
*/

/* Define size of the calling convention register save area.
   This includes room for the 16 GPR's, a saved frame size, and
   a (floating point math) scratch area */
#define I370_SAVE_AREA_SIZE 88

/* Define the size of the amount of room reserved for varargs */
#define I370_VARARGS_AREA_SIZE 512

/* Used in i370.md for temp scratch area. Must be that last two words
   of the I370_SAVE_AREA. */
#define CONVLO "80"
#define CONVHI "84"

/* Define offset from stack pointer, to location where a parm can be
   pushed.  */

#define STACK_POINTER_OFFSET I370_SAVE_AREA_SIZE

#define STACK_DYNAMIC_OFFSET(FNDECL) 0

/* Offset within frame to start allocating local variables at.
   It is the offset to the BEGINNING of the first local allocated.  */

#define STARTING_FRAME_OFFSET \
  ((current_function_varargs || current_function_stdarg) ? (I370_SAVE_AREA_SIZE + I370_VARARGS_AREA_SIZE) : (I370_SAVE_AREA_SIZE + current_function_args_size))

#define INITIAL_FRAME_POINTER_OFFSET(DEPTH) (DEPTH) = STARTING_FRAME_OFFSET

/* Offset of first incoming parameter from the arg ptr register value.  */
#define FIRST_PARM_OFFSET(FNDECL) I370_SAVE_AREA_SIZE

/* The ACCUMULATE_OUTGOING_ARGS flag seems to have some funny side effects
   that we need.  Specifically, if it is set, then the stack pointer is
   not bumped when args are placed on the stack, which is just how we want
   it. */
#define ACCUMULATE_OUTGOING_ARGS 1

#endif /* TARGET_LINUX */

/* ================= */
/* ------------------------------------------------------------------- */

/* If we generate an insn to push BYTES bytes, this says how many the stack
   pointer really advances by.  On the 370, we have no push instruction.  */

/* #define PUSH_ROUNDING(BYTES) */

#ifdef TARGET_LE
#define STACK_POINTER_OFFSET 148
#define STACK_FRAME_BASE 28
#endif

/* Offset of first parameter from the argument pointer register value.  */

#define FIRST_PARM_OFFSET(FNDECL) 0

/* 1 if N is a possible register number for function argument passing.
   On the 370, no registers are used in this way.  */

#define FUNCTION_ARG_REGNO_P(N) 0

/* Define a data type for recording info about an argument list during
   the scan of that argument list.  This data type should hold all
   necessary information about the function itself and about the args
   processed so far, enough to enable macros such as FUNCTION_ARG to
   determine where the next arg should go.  */

#define CUMULATIVE_ARGS int

/* Initialize a variable CUM of type CUMULATIVE_ARGS for a call to
   a function whose data type is FNTYPE.
   For a library call, FNTYPE is 0.  */

#define INIT_CUMULATIVE_ARGS(CUM, FNTYPE, LIBNAME, INDIRECT, ...) ((CUM) = 0)

#define ELIMINABLE_REGS                                             \
  {                                                                 \
    {FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM},                   \
        {FRAME_POINTER_REGNUM, HARD_FRAME_POINTER_REGNUM},          \
        {ARG_POINTER_REGNUM, STACK_POINTER_REGNUM},                 \
        {ARG_POINTER_REGNUM, HARD_FRAME_POINTER_REGNUM},            \
        {RETURN_ADDRESS_REGNUM, STACK_POINTER_REGNUM},      \
        {RETURN_ADDRESS_REGNUM, HARD_FRAME_POINTER_REGNUM}, \
    {                                                               \
      BASE_REGNUM, BASE_REGNUM                                      \
    }                                                               \
  }

#define TARGET_CPU_CPP_BUILTINS() i370_cpu_cpp_builtins(pfile)

/* The basic stack layout looks like this: the stack pointer points
   to the register save area for called functions.  Above that area
   is the location to place outgoing arguments.  Above those follow
   dynamic allocations (alloca), and finally the local variables.  */

/* Offset from stack-pointer to first location of outgoing args.  */
#define STACK_POINTER_OFFSET 88

/* Offset from the stack pointer register to an item dynamically
   allocated on the stack, e.g., by `alloca'.  */
#define STACK_DYNAMIC_OFFSET(FUNDECL) \
  (STACK_POINTER_OFFSET + crtl->outgoing_args_size)

/* Offset of first parameter from the argument pointer register value.
   We have a fake argument pointer register that points directly to
   the argument area.  */
#define FIRST_PARM_OFFSET(FNDECL) 0

/* Defining this macro makes __builtin_frame_address(0) and
   __builtin_return_address(0) work with -fomit-frame-pointer.  */
#define INITIAL_FRAME_ADDRESS_RTX \
  (plus_constant(Pmode, arg_pointer_rtx, -STACK_POINTER_OFFSET))

/* The return address of the current frame is retrieved
   from the initial value of register RETURN_ADDRESS_REGNUM.
   For frames farther back, we use the stack slot where
   the corresponding RETURN_ADDRESS_REGNUM register was saved.  */
#define DYNAMIC_CHAIN_ADDRESS(FRAME) \
  (1 ? plus_constant(Pmode, (FRAME), STACK_POINTER_OFFSET - UNITS_PER_LONG) : (FRAME))

/* For -mpacked-stack this adds 160 - 8 (96 - 4) to the output of
   builtin_frame_address.  Otherwise arg pointer -
   STACK_POINTER_OFFSET would be returned for
   __builtin_frame_address(0) what might result in an address pointing
   somewhere into the middle of the local variables since the packed
   stack layout generally does not need all the bytes in the register
   save area.  */
#define FRAME_ADDR_RTX(FRAME) \
  DYNAMIC_CHAIN_ADDRESS((FRAME))

/* Describe how we implement __builtin_eh_return.  */
#define EH_RETURN_DATA_REGNO(N) ((N) < 4 ? (N) + 6 : INVALID_REGNUM)
#define EH_RETURN_HANDLER_RTX gen_rtx_MEM(Pmode, return_address_pointer_rtx)

/* Update the data in CUM to advance over an argument of mode MODE and
   data type TYPE.  (TYPE is null for libcalls where that information
   may not be available.) */

/*
#define FUNCTION_ARG_ADVANCE(CUM, MODE, TYPE, NAMED)			\
 ((CUM) += ((MODE) == DFmode || (MODE) == SFmode			\
      ? 256							\
      : (MODE) != BLKmode                 			\
      ? (GET_MODE_SIZE (MODE) + 3) / 4 				\
      : (int_size_in_bytes (TYPE) + 3) / 4))
*/

/* Define where to put the arguments to a function.  Value is zero to push
   the argument on the stack, or a hard register in which to store the
   argument.  */

/*
#define FUNCTION_ARG(CUM, MODE, TYPE, NAMED) 0
*/

/* For an arg passed partly in registers and partly in memory, this is the
   number of registers used.  For args passed entirely in registers or
   entirely in memory, zero.  */

/*
#define FUNCTION_ARG_PARTIAL_NREGS(CUM, MODE, TYPE, NAMED) 0
*/

/* Define if returning from a function call automatically pops the
   arguments described by the number-of-args field in the call.  */

/*
#define RETURN_POPS_ARGS(FUNDECL,FUNTYPE,SIZE) 0
*/

/* The FUNCTION_VALUE macro defines how to find the value returned by a
   function.  VALTYPE is the data type of the value (as a tree).
   If the precise function being called is known, FUNC is its FUNCTION_DECL;
   otherwise, FUNC is NULL.

   On the 370 the return value is in R15 or R16.  However,
   DImode (64-bit ints) scalars need to get returned on the stack,
   with r15 pointing to the location.  To accomplish this, we define
   the RETURN_IN_MEMORY macro to be true for both blockmode (structures)
   and the DImode scalars.
 */

#define RET_REG(MODE) \
  (((MODE) == DCmode || (MODE) == SCmode /*|| (MODE) == TFmode*/ || (MODE) == DFmode || (MODE) == SFmode) ? 16 : 15)

#define FUNCTION_VALUE(VALTYPE, FUNC) \
  gen_rtx_REG(TYPE_MODE(VALTYPE), RET_REG(TYPE_MODE(VALTYPE)))

#define RETURN_IN_MEMORY(VALTYPE) \
  ((DImode == TYPE_MODE(VALTYPE)) || (BLKmode == TYPE_MODE(VALTYPE)))

/* Define how to find the value returned by a library function assuming
   the value has mode MODE.  */

#define LIBCALL_VALUE(MODE) gen_rtx_REG(MODE, RET_REG(MODE))

/* 1 if N is a possible register number for a function value.
   On the 370 under C/370, R15 and R16 are thus used.  */

#define FUNCTION_VALUE_REGNO_P(N) ((N) == 15 || (N) == 16)

/* This macro definition sets up a default value for `main' to return.  */

/*
#define DEFAULT_MAIN_RETURN  c_expand_return (integer_zero_node)
*/

/* Output assembler code for a block containing the constant parts of a
   trampoline, leaving space for the variable parts.

   On the 370, the trampoline contains these instructions:

        BALR  14,0
        USING *,14
        L     STATIC_CHAIN_REGISTER,X
        L     15,Y
        BR    15
   X    DS    0F
   Y    DS    0F  */
/*
   I am confused as to why this emitting raw binary, instead of instructions ...
   see for example, rs6000/rs000.c for an example of a different way to
   do this ... especially since BASR should probably be substituted for BALR.
 */

/*
#define TRAMPOLINE_TEMPLATE(FILE)					\
{									\
  assemble_aligned_integer (2, GEN_INT (0x05E0));			\
  assemble_aligned_integer (2, GEN_INT (0x5800 | STATIC_CHAIN_REGNUM << 4)); \
  assemble_aligned_integer (2, GEN_INT (0xE00A));			\
  assemble_aligned_integer (2, GEN_INT (0x58F0)); 			\
  assemble_aligned_integer (2, GEN_INT (0xE00E));			\
  assemble_aligned_integer (2, GEN_INT (0x07FF));			\
  assemble_aligned_integer (2, const0_rtx);				\
  assemble_aligned_integer (2, const0_rtx);				\
  assemble_aligned_integer (2, const0_rtx);				\
  assemble_aligned_integer (2, const0_rtx);				\
}
*/

/* Length in units of the trampoline for entering a nested function.  */

#define TRAMPOLINE_SIZE 20

/* Emit RTL insns to initialize the variable parts of a trampoline.  */

/*
#define INITIALIZE_TRAMPOLINE(TRAMP, FNADDR, CXT)			\
{									\
  emit_move_insn (gen_rtx_MEM (SImode, plus_constant (TRAMP, 12)), CXT); \
  emit_move_insn (gen_rtx_MEM (SImode, plus_constant (TRAMP, 16)), FNADDR); \
}
*/

/* Define EXIT_IGNORE_STACK if, when returning from a function, the stack
   pointer does not matter (provided there is a frame pointer).  */

#define EXIT_IGNORE_STACK 1

/* Addressing modes, and classification of registers for them.  */

/* #define HAVE_POST_INCREMENT */
/* #define HAVE_POST_DECREMENT */

/* #define HAVE_PRE_DECREMENT */
/* #define HAVE_PRE_INCREMENT */

/* These assume that REGNO is a hard or pseudo reg number.  They give
   nonzero only if REGNO is a hard reg of the suitable class or a pseudo
   reg currently allocated to a suitable hard reg.
   These definitions are NOT overridden anywhere.  */

#define REGNO_OK_FOR_INDEX_P(REGNO) \
  (((REGNO) > 0 && (REGNO) < 16) || (reg_renumber[REGNO] > 0 && reg_renumber[REGNO] < 16))

#define REGNO_OK_FOR_BASE_P(REGNO) REGNO_OK_FOR_INDEX_P(REGNO)

#define REGNO_OK_FOR_DATA_P(REGNO) \
  ((REGNO) < 16 || (unsigned)reg_renumber[REGNO] < 16)

#define REGNO_OK_FOR_FP_P(REGNO) \
  ((unsigned)((REGNO)-16) < 4 || (unsigned)(reg_renumber[REGNO] - 16) < 4)

/* Now macros that check whether X is a register and also,
   strictly, whether it is in a specified class.  */

/* 1 if X is a data register.  */

#define DATA_REG_P(X) (REG_P(X) && REGNO_OK_FOR_DATA_P(REGNO(X)))

/* 1 if X is an fp register.  */

#define FP_REG_P(X) (REG_P(X) && REGNO_OK_FOR_FP_P(REGNO(X)))

/* 1 if X is an address register.  */

#define ADDRESS_REG_P(X) (REG_P(X) && REGNO_OK_FOR_BASE_P(REGNO(X)))

/* Maximum number of registers that can appear in a valid memory address.  */

#define MAX_REGS_PER_ADDRESS 2

/* Recognize any constant value that is a valid address.  */

#define CONSTANT_ADDRESS_P(X) \
  (GET_CODE(X) == LABEL_REF || GET_CODE(X) == SYMBOL_REF || GET_CODE(X) == CONST_INT || GET_CODE(X) == CONST_DOUBLE || (GET_CODE(X) == CONST && GET_CODE(XEXP(XEXP(X, 0), 0)) == LABEL_REF) || (GET_CODE(X) == CONST && GET_CODE(XEXP(XEXP(X, 0), 0)) == SYMBOL_REF && !SYMBOL_REF_FLAG(XEXP(XEXP(X, 0), 0))))

/* Nonzero if the constant value X is a legitimate general operand.
   It is given that X satisfies CONSTANT_P or is a CONST_DOUBLE.  */

/*
#define LEGITIMATE_CONSTANT_P(X) 1
*/

/* The macros REG_OK_FOR..._P assume that the arg is a REG rtx and check
   its validity for a certain class.  We have two alternate definitions
   for each of them.  The usual definition accepts all pseudo regs; the
   other rejects them all.  The symbol REG_OK_STRICT causes the latter
   definition to be used.

   Most source files want to accept pseudo regs in the hope that they will
   get allocated to the class that the insn wants them to be in.
   Some source files that are used after register allocation
   need to be strict.  */

#ifndef REG_OK_STRICT

/* Nonzero if X is a hard reg that can be used as an index or if it is
  a pseudo reg.  */

#define REG_OK_FOR_INDEX_P(X) \
  ((REGNO(X) > 0 && REGNO(X) < 16) || REGNO(X) >= 20)

/* Nonzero if X is a hard reg that can be used as a base reg or if it is
   a pseudo reg.  */

#define REG_OK_FOR_BASE_P(X) REG_OK_FOR_INDEX_P(X)

#else /* REG_OK_STRICT */

/* Nonzero if X is a hard reg that can be used as an index.  */

#define REG_OK_FOR_INDEX_P(X) REGNO_OK_FOR_INDEX_P(REGNO(X))

/* Nonzero if X is a hard reg that can be used as a base reg.  */

#define REG_OK_FOR_BASE_P(X) REGNO_OK_FOR_BASE_P(REGNO(X))

#endif /* REG_OK_STRICT */

/* GO_IF_LEGITIMATE_ADDRESS recognizes an RTL expression that is a
   valid memory address for an instruction.
   The MODE argument is the machine mode for the MEM expression
   that wants to use this address.

   The other macros defined here are used only in GO_IF_LEGITIMATE_ADDRESS,
   except for CONSTANT_ADDRESS_P which is actually machine-independent.
*/

#define COUNT_REGS(X, REGS, FAIL)                                   \
  if (REG_P(X))                                                     \
  {                                                                 \
    if (REG_OK_FOR_BASE_P(X))                                       \
      REGS += 1;                                                    \
    else                                                            \
      goto FAIL;                                                    \
  }                                                                 \
  else if (GET_CODE(X) != CONST_INT || (unsigned)INTVAL(X) >= 4096) \
    goto FAIL;

#define GO_IF_LEGITIMATE_ADDRESS(MODE, X, ADDR) \
  {                                             \
    if (REG_P(X) && REG_OK_FOR_BASE_P(X))       \
      goto ADDR;                                \
    if (GET_CODE(X) == PLUS)                    \
    {                                           \
      int regs = 0;                             \
      rtx x0 = XEXP(X, 0);                      \
      rtx x1 = XEXP(X, 1);                      \
      if (GET_CODE(x0) == PLUS)                 \
      {                                         \
        COUNT_REGS(XEXP(x0, 0), regs, FAIL);    \
        COUNT_REGS(XEXP(x0, 1), regs, FAIL);    \
        COUNT_REGS(x1, regs, FAIL);             \
        if (regs == 2)                          \
          goto ADDR;                            \
      }                                         \
      else if (GET_CODE(x1) == PLUS)            \
      {                                         \
        COUNT_REGS(x0, regs, FAIL);             \
        COUNT_REGS(XEXP(x1, 0), regs, FAIL);    \
        COUNT_REGS(XEXP(x1, 1), regs, FAIL);    \
        if (regs == 2)                          \
          goto ADDR;                            \
      }                                         \
      else                                      \
      {                                         \
        COUNT_REGS(x0, regs, FAIL);             \
        COUNT_REGS(x1, regs, FAIL);             \
        if (regs != 0)                          \
          goto ADDR;                            \
      }                                         \
    }                                           \
  FAIL:;                                        \
  }

/* The 370 has no mode dependent addresses.  */

/*
#define GO_IF_MODE_DEPENDENT_ADDRESS(ADDR, LABEL)
*/

/* Macro: LEGITIMIZE_ADDRESS(X, OLDX, MODE, WIN)
   Try machine-dependent ways of modifying an illegitimate address
   to be legitimate.  If we find one, return the new, valid address.
   This macro is used in only one place: `memory_address' in explow.c.

   Several comments:
   (1) It's not obvious that this macro results in better code
       than its omission does. For historical reasons we leave it in.

   (2) This macro may be (???) implicated in the accidental promotion
       or RS operand to RX operands, which bombs out any RS, SI, SS
       instruction that was expecting a simple address.  Note that
       this occurs fairly rarely ...

   (3) There is a bug somewhere that causes either r4 to be spilled,
       or causes r0 to be used as a base register.  Changeing the macro
       below will make the bug move around, but will not make it go away
       ... Note that this is a rare bug ...

 */

/*
#define LEGITIMIZE_ADDRESS(X, OLDX, MODE, WIN)				\
{									\
  if (GET_CODE (X) == PLUS && CONSTANT_ADDRESS_P (XEXP (X, 1)))		\
    (X) = gen_rtx_PLUS (SImode, XEXP (X, 0),				\
      copy_to_mode_reg (SImode, XEXP (X, 1)));	\
  if (GET_CODE (X) == PLUS && CONSTANT_ADDRESS_P (XEXP (X, 0)))		\
    (X) = gen_rtx_PLUS (SImode, XEXP (X, 1),				\
      copy_to_mode_reg (SImode, XEXP (X, 0)));	\
  if (GET_CODE (X) == PLUS && GET_CODE (XEXP (X, 0)) == MULT)		\
    (X) = gen_rtx_PLUS (SImode, XEXP (X, 1),				\
      force_operand (XEXP (X, 0), 0));		\
  if (GET_CODE (X) == PLUS && GET_CODE (XEXP (X, 1)) == MULT)		\
    (X) = gen_rtx_PLUS (SImode, XEXP (X, 0),				\
      force_operand (XEXP (X, 1), 0));		\
  if (memory_address_p (MODE, X))					\
    goto WIN;								\
}
*/

/* Specify the machine mode that this machine uses for the index in the
   tablejump instruction.  */

#define CASE_VECTOR_MODE SImode

/* Define this if the tablejump instruction expects the table to contain
   offsets from the address of the table.
   Do not define this if the table should contain absolute addresses.  */

/* #define CASE_VECTOR_PC_RELATIVE */

/* Define this if fixuns_trunc is the same as fix_trunc.  */

/*
#define FIXUNS_TRUNC_LIKE_FIX_TRUNC
*/

/* We use "unsigned char" as default.  */

#define DEFAULT_SIGNED_CHAR 0

/* Max number of bytes we can move from memory to memory in one reasonably
   fast instruction.  */

#define MOVE_MAX 256

/* Nonzero if access to memory by bytes is slow and undesirable.  */

#define SLOW_BYTE_ACCESS 1

/* Define if shifts truncate the shift count which implies one can omit
   a sign-extension or zero-extension of a shift count.  */

/* #define SHIFT_COUNT_TRUNCATED */

/* Value is 1 if truncating an integer of INPREC bits to OUTPREC bits
   is done just by pretending it is already truncated.  */

/*
#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC)	(OUTPREC != 16)
*/

/* We assume that the store-condition-codes instructions store 0 for false
   and some other value for true.  This is the value stored for true.  */

/* #define STORE_FLAG_VALUE (-1) */

/* When a prototype says `char' or `short', really pass an `int'.  */

/*
#define PROMOTE_PROTOTYPES 1
*/

/* Don't perform CSE on function addresses.  */
/* It is as good or better to call a constant function address than to
   call an address kept in a register.  */
#define NO_FUNCTION_CSE 0

/* Specify the machine mode that pointers have.
   After generation of rtl, the compiler makes no further distinction
   between pointers and any other objects of this machine mode.  */

/* Theoretically using DImode instead of SImode should stop
   the negative indexes being generated, which are a problem
   if we run a 32-bit executable while in AMODE64, but there
   seems to be something else broken in the i370 target preventing
   that from working */
/* #define Pmode DImode */
#define Pmode SImode

/* A function address in a call instruction is a byte address (for
   indexing purposes) so give the MEM rtx a byte's mode.  */

#define FUNCTION_MODE QImode

/* Compute the cost of computing a constant rtl expression RTX whose
   rtx-code is CODE.  The body of this macro is a portion of a switch
   statement.  If the code is computed here, return it with a return
   statement.  Otherwise, break from the switch.  */

/*
#define CONST_COSTS(RTX, CODE, OUTERCODE)				\
  case CONST_INT:							\
    if ((unsigned) INTVAL (RTX) < 0xfff) return 1;			\
  case CONST:								\
  case LABEL_REF:							\
  case SYMBOL_REF:							\
    return 2;								\
  case CONST_DOUBLE:							\
    return 4;
*/

/*   A C statement (sans semicolon) to update the integer variable COST
     based on the relationship between INSN that is dependent on
     DEP_INSN through the dependence LINK.  The default is to make no
     adjustment to COST.  This can be used for example to specify to
     the scheduler that an output- or anti-dependence does not incur
     the same cost as a data-dependence.

     We will want to use this to indicate that there is a cost associated
     with the loading, followed by use of base registers ...
#define ADJUST_COST (INSN, LINK, DEP_INSN, COST)
 */

/* Tell final.c how to eliminate redundant test instructions.  */

/* Here we define machine-dependent flags and fields in cc_status
   (see `conditions.h').  */

/* Store in cc_status the expressions that the condition codes will
   describe after execution of an instruction whose pattern is EXP.
   Do not alter them if the instruction would not alter the cc's.

   On the 370, load insns do not alter the cc's.  However, in some
   cases these instructions can make it possibly invalid to use the
   saved cc's.  In those cases we clear out some or all of the saved
   cc's so they won't be used.

   Note that only some arith instructions set the CC.  These include
   add, subtract, complement, various shifts.  Note that multiply
   and divide do *not* set set the CC.  Therefore, in the code below,
   don't set the status for MUL, DIV, etc.

   Note that the bitwise ops set the condition code, but not in a
   way that we can make use of it. So we treat these as clobbering,
   rather than setting the CC.  These are clobbered in the individual
   instruction patterns that use them.  Use CC_STATUS_INIT to clobber.
*/

#if 0
#define NOTICE_UPDATE_CC(EXP, INSN)                                              \
  {                                                                              \
    rtx exp = (EXP);                                                             \
    if (GET_CODE(exp) == PARALLEL) /* Check this */                              \
      exp = XVECEXP(exp, 0, 0);                                                  \
    if (GET_CODE(exp) != SET)                                                    \
      CC_STATUS_INIT;                                                            \
    else                                                                         \
    {                                                                            \
      if (XEXP(exp, 0) == cc0_rtx)                                               \
      {                                                                          \
        cc_status.value1 = XEXP(exp, 0);                                         \
        cc_status.value2 = XEXP(exp, 1);                                         \
        cc_status.flags = 0;                                                     \
      }                                                                          \
      else                                                                       \
      {                                                                          \
        if (cc_status.value1 && reg_mentioned_p(XEXP(exp, 0), cc_status.value1)) \
          cc_status.value1 = 0;                                                  \
        if (cc_status.value2 && reg_mentioned_p(XEXP(exp, 0), cc_status.value2)) \
          cc_status.value2 = 0;                                                  \
        switch (GET_CODE(XEXP(exp, 1)))                                          \
        {                                                                        \
        case PLUS:                                                               \
        case MINUS:                                                              \
        case NEG:                                                                \
        case NOT:                                                                \
        case ABS:                                                                \
          CC_STATUS_SET(XEXP(exp, 0), XEXP(exp, 1));                             \
                                                                                 \
          /* mult and div don't set any cc codes !! */                           \
        case MULT: /* case UMULT: */                                             \
        case DIV:                                                                \
        case UDIV:                                                               \
          /* and, or and xor set the cc's the wrong way !! */                    \
        case AND:                                                                \
        case IOR:                                                                \
        case XOR:                                                                \
        /* some shifts set the CC some don't. */                                 \
        case ASHIFT:                                                             \
        case ASHIFTRT:                                                           \
          do                                                                     \
          {                                                                      \
          } while (0);                                                           \
        default:                                                                 \
          break;                                                                 \
        }                                                                        \
      }                                                                          \
    }                                                                            \
  }
#endif

struct i370_cc_flags
{
  int flags;
  const_rtx value1;
  const_rtx value2;
};
extern struct i370_cc_flags cc_status;

#define CC_STATUS_SET(V1, V2)                                                    \
  {                                                                              \
    cc_status.flags = 0;                                                         \
    cc_status.value1 = (V1);                                                     \
    cc_status.value2 = (V2);                                                     \
    if (cc_status.value1 && reg_mentioned_p(cc_status.value1, cc_status.value2)) \
      cc_status.value2 = 0;                                                      \
  }

#define CC_STATUS_INIT   \
  {                      \
    cc_status.flags = 0; \
  }

#define OUTPUT_JUMP(NORMAL, FLOAT, NO_OV) \
  {                                       \
    if (cc_status.flags & CC_NO_OVERFLOW) \
      return NO_OV;                       \
    return NORMAL;                        \
  }

/* Define the offset between two registers, one to be eliminated, and the other
   its replacement, at the start of a routine.  */
#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET) \
  ((OFFSET) = 0)

/* ------------------------------------------ */
/* Control the assembler format that we output.  */

/* Define standard character escape sequences for non-ASCII targets
   only.  */

/* ======================================================== */

#ifdef TARGET_HLASM

#define TEXT_SECTION_ASM_OP "* Program text area"
#define DATA_SECTION_ASM_OP "* Program data area"
#define INIT_SECTION_ASM_OP "* Program initialization area"
#define FINI_SECTION_ASM_OP "* Program finalization area"
#define CTOR_LIST_BEGIN /* NO OP */
#define CTOR_LIST_END   /* NO OP */
#define MAX_MVS_LABEL_SIZE 8

/* How to refer to registers in assembler output.  This sequence is
   indexed by compiler's hard-register-number (see above).  */

#define REGISTER_NAMES                                          \
  {                                                             \
    "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7",             \
        "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15",   \
        "F0", "F2", "F4", "F6", "F1", "F3", "F5", "F7",         \
        "F8", "F10", "F12", "F14", "F9", "F11", "F13", "F15",   \
        "AP", "CC", "FP", "RP", "A0", "A1",                     \
        "V16", "V18", "V20", "V22", "V17", "V19", "V21", "V23", \
        "V24", "V26", "V28", "V30", "V25", "V27", "V29", "V31"  \
  }

#define ADDITIONAL_REGISTER_NAMES \
  {{"V0", 16}, {"V2", 17}, {"V4", 18}, {"V6", 19}, {"V1", 20}, {"V3", 21}, {"V5", 22}, {"V7", 23}, {"V8", 24}, {"V10", 25}, {"V12", 26}, {"V14", 27}, {"V9", 28}, {"V11", 29}, {"V13", 30}, {"V15", 31}};

#ifdef TARGET_ALIASES
#define TARGET_ASM_FILE_END(FILE) \
  {                               \
    mvs_dump_alias(FILE);         \
    fputs("\tEND\n", FILE);       \
  }
#else

#define TARGET_ASM_FILE_START i370_asm_start
#define TARGET_ASM_FILE_END i370_asm_end

#endif /* TARGET_ALIASES */

#define ASM_COMMENT_START "*"
#define ASM_APP_OFF ""
#define ASM_APP_ON ""

#define ASM_OUTPUT_LABEL(FILE, NAME) \
  {                                  \
    assemble_name(FILE, NAME);       \
    fputs("\tEQU\t*\n", FILE);       \
  }

#define ASM_OUTPUT_FUNCTION_PREFIX(FILE, NAME) \
  mvs_need_to_globalize = 0;                   \
  mvs_need_entry = 0

#if defined(TARGET_DIGNUS) || defined(TARGET_PDPMAC)
#define ASM_OUTPUT_EXTERNAL(FILE, DECL, NAME)
#endif
#ifdef TARGET_LE
#define ASM_OUTPUT_EXTERNAL(FILE, DECL, NAME)          \
  {                                                    \
    char temp[MAX_MVS_LABEL_SIZE + 1];                 \
    if (mvs_check_alias(NAME, temp) == 2)              \
    {                                                  \
      fprintf(FILE, "%s\tALIAS\tC'%s'\n", temp, NAME); \
    }                                                  \
  }
#endif

#define TARGET_ASM_GLOBALIZE_LABEL i370_asm_globalize_label

/* MVS externals are limited to 8 characters, upper case only.
   The '_' is mapped to '@', except for MVS functions, then '#'.  */
#define ASM_OUTPUT_LABELREF(FILE, NAME) i370_asm_label_ref

#define ASM_GENERATE_INTERNAL_LABEL(LABEL, PREFIX, NUM) \
  sprintf(LABEL, "*@@%s%i", PREFIX, (int)NUM)

/* Generate internal label.  Since we can branch here from off page, we
   must reload the base register.  */

#define ASM_OUTPUT_INTERNAL_LABEL(FILE, NUM) \
  {                                          \
    mvs_add_label(0);                        \
    fprintf(FILE, "@@L%d\tEQU\t*\n", NUM);   \
  }

/* Generate case label.  For HLASM we can change to the data CSECT
   and put the vectors out of the code body. The assembler just
   concatenates CSECTs with the same name.  */

#ifdef TARGET_ALIASES
#define ASM_OUTPUT_CASE_LABEL(FILE, PREFIX, NUM, TABLE) \
  fprintf(FILE, "\tDS\t0F\n");                          \
  fprintf(FILE, "@DATA\tCSECT\n");                      \
  fprintf(FILE, "%s%d\tEQU\t*\n", PREFIX, NUM)
#else /* !TARGET_ALIASES */
#ifdef TARGET_PDPMAC
#define ASM_OUTPUT_CASE_LABEL(FILE, PREFIX, NUM, TABLE) \
  fprintf(FILE, "\tLTORG\n");                           \
  fprintf(FILE, "\tDS\t0F\n");                          \
  mvs_case_code = 0;                                    \
  fprintf(FILE, "@@%s%d\tEQU\t*\n", PREFIX, NUM)
#else
#define ASM_OUTPUT_CASE_LABEL(FILE, PREFIX, NUM, TABLE) \
  fprintf(FILE, "\tDS\t0F\n");                          \
  fprintf(FILE, "$%s\tCSECT\n", mvs_module);            \
  fprintf(FILE, "%s%d\tEQU\t*\n", PREFIX, NUM)
#endif
#endif /* TARGET_ALIASES */

/* Put the CSECT back to the code body */

#ifdef TARGET_ALIASES
#define ASM_OUTPUT_CASE_END(FILE, NUM, TABLE) \
  fputs("@CODE\tCSECT\n", FILE);
#else /* !TARGET_ALIASES */
#ifdef TARGET_PDPMAC
#define ASM_OUTPUT_CASE_END(FILE, NUM, TABLE) \
  mvs_page_code += mvs_case_code;             \
  mvs_check_page(FILE, 0, 0);                 \
  mvs_case_code = 0;
#else
#define ASM_OUTPUT_CASE_END(FILE, NUM, TABLE) \
  fprintf(FILE, "@%s\tCSECT\n", mvs_module);
#endif
#endif /* TARGET_ALIASES */

/* This is how to output an element of a case-vector that is absolute.  */

#define ASM_OUTPUT_ADDR_VEC_ELT(FILE, VALUE)  \
  do {                                        \
    mvs_case_code += 4;                       \
    fprintf(FILE, "\tDC\tA(@@L%d)\n", VALUE); \
  } while(0)

/* This is how to output an element of a case-vector that is relative.  */

#define ASM_OUTPUT_ADDR_DIFF_ELT(FILE, BODY, VALUE, REL) \
  fprintf(FILE, "\tDC\tA(@@L%d-@@L%d)\n", VALUE, REL)

/* This is how to output an insn to push a register on the stack.
    It need not be very fast code.
   Right now, PUSH & POP are used only when profiling is enabled,
   and then, only to push the static chain reg and the function struct
   value reg, and only if those are used.  Since profiling is not
   supported anyway, punt on this.  */

#define ASM_OUTPUT_REG_PUSH(FILE, REGNO)              \
  do {                                                \
    mvs_check_page(FILE, 8, 4);                       \
    fprintf(FILE, "\tS\t13,=F'4'\n\tST\t%s,%d(13)\n", \
            reg_names[REGNO], STACK_POINTER_OFFSET);  \
  } while(0)

/* This is how to output an insn to pop a register from the stack.
   It need not be very fast code.  */

#define ASM_OUTPUT_REG_POP(FILE, REGNO)               \
  do {                                                \
    mvs_check_page(FILE, 8, 0);                       \
    fprintf(FILE, "\tL\t%s,%d(13)\n\tLA\t13,4(13)\n", \
            reg_names[REGNO], STACK_POINTER_OFFSET);  \
  } while(0)

/* This outputs a text string.  The string are chopped up to fit into
   an 80 byte record.  Also, control and special characters, interpreted
   by the IBM assembler, are output numerically.  */

#define MVS_ASCII_TEXT_LENGTH 48

#define ASM_OUTPUT_ASCII(FILE, PTR, LEN)                              \
  {                                                                   \
    size_t i, limit = (LEN);                                          \
    int j;                                                            \
    for (j = 0, i = 0; i < limit; j++, i++)                           \
    {                                                                 \
      int c = (PTR)[i];                                               \
      c = MAP_INCHAR(c);                                              \
      if (!IS_ISOBASIC(c) || ISCNTRL(c) || c == '&')                  \
      {                                                               \
        if (j % MVS_ASCII_TEXT_LENGTH != 0)                           \
          fprintf(FILE, "'\n");                                       \
        j = -1;                                                       \
        c = MAP_OUTCHAR(c);                                           \
        fprintf(FILE, "\tDC\tX'%X'\n", c);                            \
      }                                                               \
      else                                                            \
      {                                                               \
        if (j % MVS_ASCII_TEXT_LENGTH == 0)                           \
          fprintf(FILE, "\tDC\tC'");                                  \
        if (c == '\'')                                                \
        { /* we are going to print 2 chars - is there space */        \
          if (j % MVS_ASCII_TEXT_LENGTH == MVS_ASCII_TEXT_LENGTH - 1) \
          { /* not enough space */                                    \
            fprintf(FILE, "'\n");                                     \
            fprintf(FILE, "\tDC\tC'");                                \
            ++j;                                                      \
          }                                                           \
          fprintf(FILE, "%c%c", c, c);                                \
          ++j;                                                        \
        }                                                             \
        else                                                          \
          fprintf(FILE, "%c", c);                                     \
        if (j % MVS_ASCII_TEXT_LENGTH == MVS_ASCII_TEXT_LENGTH - 1)   \
          fprintf(FILE, "'\n");                                       \
      }                                                               \
    }                                                                 \
    if (j % MVS_ASCII_TEXT_LENGTH != 0)                               \
      fprintf(FILE, "'\n");                                           \
  }

/* This is how to output an assembler line that says to advance the
   location counter to a multiple of 2**LOG bytes.  */

#define ASM_OUTPUT_ALIGN(FILE, LOG) \
  if (LOG)                          \
  {                                 \
    if ((LOG) == 1)                 \
      fprintf(FILE, "\tDS\t0H\n");  \
    else                            \
      fprintf(FILE, "\tDS\t0F\n");  \
  }

/* The maximum length of memory that the IBM assembler will allow in one
   DS operation.  */

#define MAX_CHUNK 32767

/* A C statement to output to the stdio stream FILE an assembler
   instruction to advance the location counter by SIZE bytes. Those
   bytes should be zero when loaded.  */

#ifdef TARGET_PDPMAC
#define ASM_OUTPUT_SKIP(FILE, SIZE)         \
  {                                         \
    int s, k;                               \
    for (s = (SIZE); s > 0; s -= MAX_CHUNK) \
    {                                       \
      if (s > MAX_CHUNK)                    \
        k = MAX_CHUNK;                      \
      else                                  \
        k = s;                              \
      fprintf(FILE, "\tDC\t%dX'00'\n", k);  \
    }                                       \
  }
#else
#define ASM_OUTPUT_SKIP(FILE, SIZE)         \
  {                                         \
    int s, k;                               \
    for (s = (SIZE); s > 0; s -= MAX_CHUNK) \
    {                                       \
      if (s > MAX_CHUNK)                    \
        k = MAX_CHUNK;                      \
      else                                  \
        k = s;                              \
      fprintf(FILE, "\tDS\tXL%d\n", k);     \
    }                                       \
  }
#endif

/* A C statement (sans semicolon) to output to the stdio stream
   FILE the assembler definition of a common-label named NAME whose
   size is SIZE bytes.  The variable ROUNDED is the size rounded up
   to whatever alignment the caller wants.  */

#define ASM_OUTPUT_COMMON(FILE, NAME, SIZE, ROUNDED)   \
  {                                                    \
    char temp[MAX_MVS_LABEL_SIZE + 1];                 \
    if (mvs_check_alias(NAME, temp) == 2)              \
    {                                                  \
      fprintf(FILE, "%s\tALIAS\tC'%s'\n", temp, NAME); \
    }                                                  \
    fprintf(FILE, "* X-var %s\n", NAME);               \
    fputs("\tENTRY\t", FILE);                          \
    assemble_name(FILE, NAME);                         \
    fputs("\n", FILE);                                 \
    fprintf(FILE, "\tDS\t0F\n");                       \
    ASM_OUTPUT_LABEL(FILE, NAME);                      \
    ASM_OUTPUT_SKIP(FILE, SIZE);                       \
  }

/* A C statement (sans semicolon) to output to the stdio stream
   FILE the assembler definition of a local-common-label named NAME
   whose size is SIZE bytes.  The variable ROUNDED is the size
   rounded up to whatever alignment the caller wants.  */

#define ASM_OUTPUT_LOCAL(FILE, NAME, SIZE, ROUNDED) \
  do {                                              \
    fprintf(FILE, "\tDS\t0F\n");                    \
    ASM_OUTPUT_LABEL(FILE, NAME);                   \
    ASM_OUTPUT_SKIP(FILE, SIZE);                    \
  } while(0)

/* Store in OUTPUT a string (made with alloca) containing an
   assembler-name for a local static variable named NAME.
   LABELNO is an integer which is different for each call.  */

#ifdef TARGET_PDPMAC
#define ASM_FORMAT_PRIVATE_NAME(OUTPUT, NAME, LABELNO) \
  do {                                                 \
    (OUTPUT) = (char *)alloca(strlen((NAME)) + 10);    \
    sprintf((OUTPUT), "__%i", (int)(LABELNO));         \
  } while(0)
#else
#define ASM_FORMAT_PRIVATE_NAME(OUTPUT, NAME, LABELNO) \
  do {                                                 \
    (OUTPUT) = (char *)alloca(strlen((NAME)) + 10);    \
    sprintf((OUTPUT), "%s%i", (NAME), (int)(LABELNO)); \
  } while(0)
#endif

/* Print operand XV (an rtx) in assembler syntax to file FILE.
   CODE is a letter or dot (`z' in `%z0') or 0 if no letter was specified.
   For `%' followed by punctuation, CODE is the punctuation and XV is null.  */

#define PRINT_OPERAND(FILE, XV, CODE) i370_print_operand(FILE, XV, CODE)

#define PRINT_OPERAND_ADDRESS(FILE, ADDR)                        \
  {                                                              \
    rtx breg, xreg, offset, plus;                                \
                                                                 \
    switch (GET_CODE(ADDR))                                      \
    {                                                            \
    case REG:                                                    \
      fprintf(FILE, "0(%s)", reg_names[REGNO(ADDR)]);            \
      break;                                                     \
    case PLUS:                                                   \
      breg = 0;                                                  \
      xreg = 0;                                                  \
      offset = 0;                                                \
      if (GET_CODE(XEXP(ADDR, 0)) == PLUS)                       \
      {                                                          \
        if (GET_CODE(XEXP(ADDR, 1)) == REG)                      \
          breg = XEXP(ADDR, 1);                                  \
        else                                                     \
          offset = XEXP(ADDR, 1);                                \
        plus = XEXP(ADDR, 0);                                    \
      }                                                          \
      else                                                       \
      {                                                          \
        if (GET_CODE(XEXP(ADDR, 0)) == REG)                      \
          breg = XEXP(ADDR, 0);                                  \
        else                                                     \
          offset = XEXP(ADDR, 0);                                \
        plus = XEXP(ADDR, 1);                                    \
      }                                                          \
      if (GET_CODE(plus) == PLUS)                                \
      {                                                          \
        if (GET_CODE(XEXP(plus, 0)) == REG)                      \
        {                                                        \
          if (breg)                                              \
            xreg = XEXP(plus, 0);                                \
          else                                                   \
            breg = XEXP(plus, 0);                                \
        }                                                        \
        else                                                     \
        {                                                        \
          offset = XEXP(plus, 0);                                \
        }                                                        \
        if (GET_CODE(XEXP(plus, 1)) == REG)                      \
        {                                                        \
          if (breg)                                              \
            xreg = XEXP(plus, 1);                                \
          else                                                   \
            breg = XEXP(plus, 1);                                \
        }                                                        \
        else                                                     \
        {                                                        \
          offset = XEXP(plus, 1);                                \
        }                                                        \
      }                                                          \
      else if (GET_CODE(plus) == REG)                            \
      {                                                          \
        if (breg)                                                \
          xreg = plus;                                           \
        else                                                     \
          breg = plus;                                           \
      }                                                          \
      else                                                       \
      {                                                          \
        offset = plus;                                           \
      }                                                          \
      if (offset)                                                \
      {                                                          \
        if (GET_CODE(offset) == LABEL_REF)                       \
          fprintf(FILE, "@@L%d",                                 \
                  CODE_LABEL_NUMBER(XEXP(offset, 0)));           \
        else                                                     \
          output_addr_const(FILE, offset);                       \
      }                                                          \
      else                                                       \
        fprintf(FILE, "0");                                      \
      if (xreg)                                                  \
        fprintf(FILE, "(%s,%s)",                                 \
                reg_names[REGNO(xreg)], reg_names[REGNO(breg)]); \
      else                                                       \
        fprintf(FILE, "(%s)", reg_names[REGNO(breg)]);           \
      break;                                                     \
    default:                                                     \
      mvs_page_lit += 4;                                         \
      if (SYMBOL_REF_FLAG(ADDR))                                 \
      {                                                          \
        fprintf(FILE, "=V(");                                    \
        output_addr_const(FILE, ADDR);                           \
        fprintf(FILE, ")");                                      \
        mvs_mark_alias(XSTR(ADDR, 0));                           \
      }                                                          \
      else                                                       \
      {                                                          \
        fprintf(FILE, "=A(");                                    \
        output_addr_const(FILE, ADDR);                           \
        fprintf(FILE, ")");                                      \
      }                                                          \
      break;                                                     \
    }                                                            \
  }

#ifdef TARGET_LE
#define ASM_DECLARE_FUNCTION_NAME(FILE, NAME, DECL)                  \
  {                                                                  \
    /* Save a copy of the function name. We need it later */         \
    if (strlen(NAME) + 1 > mvs_function_name_length)                 \
    {                                                                \
      if (mvs_function_name)                                         \
        free(mvs_function_name);                                     \
      mvs_function_name = 0;                                         \
    }                                                                \
    if (!mvs_function_name)                                          \
    {                                                                \
      mvs_function_name_length = strlen(NAME) * 2 + 1;               \
      mvs_function_name = (char *)xmalloc(mvs_function_name_length); \
    }                                                                \
    if (!strcmp(NAME, "main"))                                       \
      strcpy(mvs_function_name, "gccmain");                          \
    else                                                             \
      strcpy(mvs_function_name, NAME);                               \
  }
#endif

#if defined(TARGET_DIGNUS) || defined(TARGET_PDPMAC)
#define ASM_DECLARE_FUNCTION_NAME(FILE, NAME, DECL)                  \
  {                                                                  \
    if (strlen(NAME) + 1 > mvs_function_name_length)                 \
    {                                                                \
      if (mvs_function_name)                                         \
        free(mvs_function_name);                                     \
      mvs_function_name = 0;                                         \
    }                                                                \
    if (!mvs_function_name)                                          \
    {                                                                \
      mvs_function_name_length = strlen(NAME) * 2 + 1;               \
      mvs_function_name = (char *)xmalloc(mvs_function_name_length); \
    }                                                                \
    strcpy(mvs_function_name, NAME);                                 \
    mvs_need_to_globalize = 1;                                       \
  }
#endif

/* Output assembler code to FILE to increment profiler label # LABELNO
   for profiling a function entry.  */

#define FUNCTION_PROFILER(FILE, LABELNO) \
  fprintf(FILE, "Error: No profiling available.\n")

#endif /* TARGET_HLASM */

#endif /* ! GCC_I370_H */
