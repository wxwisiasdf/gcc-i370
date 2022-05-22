
(define_predicate "s_operand"
  (and (match_code "subreg, mem")
       (match_operand 0 "general_operand"))
{
  /* Just like memory_operand, allow (subreg (mem ...))
     after reload.  */
  if (reload_completed
      && GET_CODE (op) == SUBREG
      && GET_CODE (SUBREG_REG (op)) == MEM)
    op = SUBREG_REG (op);

  if (GET_CODE (op) != MEM)
    return false;
  /*if (!s390_legitimate_address_without_index_p (op))
    return false;*/

  return true;
})


(define_predicate "r_or_s_operand"
  (and (match_code "subreg, mem")
       (match_operand 0 "general_operand"))
{
  if (CONSTANT_ADDRESS_P (op))
    return 1;
  if (mode == VOIDmode || GET_MODE (op) != mode)
    return 0;
  if (GET_CODE (op) == REG)
    return 1;
  else if (GET_CODE (op) == MEM)
    {
      register rtx x = XEXP (op, 0);

      if (!volatile_ok && op->volatil)
	return 0;
      if (REG_P (x) && REG_OK_FOR_BASE_P (x))
	return 1;
      if (GET_CODE (x) == PLUS
	  && REG_P (XEXP (x, 0)) && REG_OK_FOR_BASE_P (XEXP (x, 0))
	  && GET_CODE (XEXP (x, 1)) == CONST_INT
	  && (unsigned) INTVAL (XEXP (x, 1)) < 4096)
	return 1;
    }
  return 0;
})
