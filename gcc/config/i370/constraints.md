;;
;;  Register constraints.
;;

(define_register_constraint "a"
  "ADDR_REGS"
  "Any address register from 1 to 15.")


;(define_register_constraint "c"
;  "CC_REGS"
;  "Condition code register 33")


(define_register_constraint "d"
  "GENERAL_REGS"
  "Any register from 0 to 15")


(define_register_constraint "f"
  "FP_REGS"
  "Floating point registers")


;(define_register_constraint "t"
;  "ACCESS_REGS"
;  "@internal
;   Access registers 36 and 37")

;;
;;  General constraints for constants.
;;

(define_constraint "C"
  "@internal
   An 8-bit signed immediate constant (-128..127)"
  (and (match_code "const_int")
       (match_test "ival >= -128 && ival <= 127")))

(define_constraint "D"
  "An unsigned 16-bit constant (0..65535)"
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival <= 65535")))

(define_constraint "I"
  "An 8-bit constant (0..255)"
  (and (match_code "const_int")
       (match_test "(unsigned HOST_WIDE_INT) ival <= 255")))


(define_constraint "J"
  "A 12-bit constant (0..4095)"
  (and (match_code "const_int")
       (match_test "(unsigned HOST_WIDE_INT) ival <= 4095")))


(define_constraint "K"
  "A 16-bit constant (-32768..32767)"
  (and (match_code "const_int")
       (match_test "ival >= -32768 && ival <= 32767")))

(define_constraint "S"
  "Some S constraint lol"
  (and (match_code "const_int")
       (match_test "s_operand (op, GET_MODE(op)) == 1")))

(define_constraint "R"
  "Some S constraint lol"
  (and (match_code "const_int")
       (match_test "r_or_s_operand (op, GET_MODE(op)) == 1")))

(define_constraint "L"
  "Value appropriate as displacement.
      (0..4095) for short displacement
      (-524288..524287) for long displacement"
  (and (match_code "const_int")
       (match_test "0 ?
              (ival >= -524288 && ival <= 524287)
            : (ival >= 0 && ival <= 4095)")))


(define_constraint "M"
  "Constant integer with a value of 0x7fffffff"
  (and (match_code "const_int")
       (match_test "ival == 2147483647")))

;(define_address_constraint "Y"
;  "Address style operand without index register"
