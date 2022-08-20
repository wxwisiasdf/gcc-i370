/* Prototypes for exported functions defined in m68hc11.c
   Copyright (C) 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2007
   Free Software Foundation, Inc.
   Contributed by Stephane Carrez (stcarrez@nerim.fr)

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef GCC_M68HC11_PROTOS_H
#define GCC_M68HC11_PROTOS_H

#ifdef RTX_CODE
extern const struct attribute_spec m68hc11_attribute_table[];

extern bool m68hc11_modes_tieable_p (machine_mode mode1, machine_mode mode2);
extern int m68hc11_override_options (void);
extern int m68hc11_optimization_options (int,int);
extern void m68hc11_conditional_register_usage (void);
extern bool m68hc11_hard_regno_mode_ok (unsigned int regno, machine_mode mode);
extern int m68hc11_hard_regno_rename_ok (int, int);

extern int m68hc11_total_frame_size (void);
extern int m68hc11_initial_frame_pointer_offset (void);
extern int m68hc11_initial_elimination_offset (int, int);

extern void m68hc11_asm_out_constructor (rtx symbol, int priority);
extern void m68hc11_asm_out_destructor (rtx symbol, int priority);
extern void m68hc11_output_function_epilogue (FILE *out);
extern void m68hc11_init_libfuncs (void);
extern unsigned int m68hc11_section_type_flags (tree decl, const char *name, int reloc);
extern void m68hc11_encode_section_info (tree decl, rtx rtl, int first);
extern const char *m68hc11_strip_name_encoding (const char *str);
extern void m68hc11_encode_label (tree decl);
extern bool m68hc11_rtx_costs (rtx x, machine_mode code, int outer_code, int opno, int *total, bool speed);
extern int m68hc11_address_cost (rtx x, machine_mode mode, addr_space_t as, bool speed);
extern void m68hc11_reorg (void);
extern rtx m68hc11_struct_value_rtx (tree fntype, int incoming);
extern bool m68hc11_return_in_memory (const_tree type, const_tree fntype);
extern void m68hc11_file_start (void);

extern void expand_prologue (void);
extern void expand_epilogue (void);

extern void m68hc11_function_arg_advance (CUMULATIVE_ARGS*,
                                          machine_mode,
                                          tree,
                                          int);

extern int m68hc11_auto_inc_p (rtx);

extern void m68hc11_initialize_trampoline (rtx, rtx, rtx);

extern rtx m68hc11_expand_compare_and_branch (enum rtx_code, rtx, rtx, rtx);
extern enum reg_class preferred_reload_class (rtx, enum reg_class);

extern int m68hc11_go_if_legitimate_address (rtx, machine_mode, int);

extern int m68hc11_legitimize_address (rtx*, rtx, machine_mode);

extern void m68hc11_notice_update_cc (rtx, rtx);
extern void m68hc11_notice_keep_cc (rtx);

extern void m68hc11_gen_movqi (rtx, rtx*);
extern void m68hc11_gen_movhi (rtx, rtx*);
extern void m68hc11_gen_rotate (enum rtx_code, rtx, rtx*);

extern void m68hc11_output_swap (rtx, rtx*);

extern int next_insn_test_reg (rtx, rtx);

extern void print_operand (FILE*, rtx, int);
extern void print_operand_address (FILE*, rtx);

extern int m68hc11_reload_operands (rtx*);

extern int dead_register_here (rtx, rtx);

extern int push_pop_operand_p (rtx);
extern void m68hc11_split_move (rtx, rtx, rtx);
extern void m68hc11_split_compare_and_branch (enum rtx_code,
                                              rtx, rtx, rtx);

extern rtx m68hc11_gen_lowpart (machine_mode, rtx);
extern rtx m68hc11_gen_highpart (machine_mode, rtx);

extern int m68hc11_memory_move_cost (machine_mode, enum reg_class, int);
extern int m68hc11_register_move_cost (machine_mode,
                                       enum reg_class, enum reg_class);

extern void m68hc11_emit_libcall (const char*, enum rtx_code,
                                  machine_mode, machine_mode,
                                  int, rtx*);
extern int m68hc11_small_indexed_indirect_p (rtx, machine_mode);
extern int m68hc11_symbolic_p (rtx, machine_mode);
extern int m68hc11_indirect_p (rtx, machine_mode);
extern int go_if_legitimate_address2 (rtx, machine_mode, int);

extern int reg_or_indexed_operand (rtx,machine_mode);
extern int memory_indexed_operand (rtx, machine_mode);

extern void m68hc11_split_logical (machine_mode, int, rtx*);

extern int m68hc11_register_indirect_p (rtx, machine_mode);
extern int m68hc11_valid_addressing_p (rtx, machine_mode, int);

extern int symbolic_memory_operand (rtx, machine_mode);

extern int memory_reload_operand (rtx, machine_mode);
extern int arith_src_operand (rtx, machine_mode);
extern int soft_reg_operand (rtx, machine_mode);

extern void m68hc11_init_cumulative_args (CUMULATIVE_ARGS*, tree, rtx);
extern pad_direction m68hc11_function_arg_padding (machine_mode mode, const_tree type);

extern void m68hc11_function_epilogue (FILE*,int);

extern int m68hc11_is_far_symbol (rtx);
extern int m68hc11_is_trap_symbol (rtx);
extern int m68hc11_page0_symbol_p (rtx x);

extern HOST_WIDE_INT m68hc11_min_offset;
extern HOST_WIDE_INT m68hc11_max_offset;
extern int m68hc11_addr_mode;

#endif /* RTX_CODE */

#endif
