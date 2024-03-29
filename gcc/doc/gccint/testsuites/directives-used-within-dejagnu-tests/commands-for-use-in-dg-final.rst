..
  Copyright 1988-2022 Free Software Foundation, Inc.
  This is part of the GCC manual.
  For copying conditions, see the copyright.rst file.

.. _final-actions:

Commands for use in dg-final
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The GCC testsuite defines the following directives to be used within
``dg-final``.

Scan a particular file
~~~~~~~~~~~~~~~~~~~~~~

:samp:`scan-file {filename}{regexp} [{ target/xfail {selector} }]`
  Passes if :samp:`{regexp}` matches text in :samp:`{filename}`.

:samp:`scan-file-not {filename}{regexp} [{ target/xfail {selector} }]`
  Passes if :samp:`{regexp}` does not match text in :samp:`{filename}`.

:samp:`scan-module {module}{regexp} [{ target/xfail {selector} }]`
  Passes if :samp:`{regexp}` matches in Fortran module :samp:`{module}`.

:samp:`dg-check-dot {filename}`
  Passes if :samp:`{filename}` is a valid :samp:`.dot` file (by running
  ``dot -Tpng`` on it, and verifying the exit code is 0).

:samp:`scan-sarif-file {regexp} [{ target/xfail {selector} }]`
  Passes if :samp:`{regexp}` matches text in the file generated by
  :option:`-fdiagnostics-format=sarif-file`.

:samp:`scan-sarif-file-not {regexp} [{ target/xfail {selector} }]`
  Passes if :samp:`{regexp}` does not match text in the file generated by
  :option:`-fdiagnostics-format=sarif-file`.

Scan the assembly output
~~~~~~~~~~~~~~~~~~~~~~~~

:samp:`scan-assembler {regex} [{ target/xfail {selector} }]`
  Passes if :samp:`{regex}` matches text in the test's assembler output.

:samp:`scan-assembler-not {regex} [{ target/xfail {selector} }]`
  Passes if :samp:`{regex}` does not match text in the test's assembler output.

:samp:`scan-assembler-times {regex}{num} [{ target/xfail {selector} }]`
  Passes if :samp:`{regex}` is matched exactly :samp:`{num}` times in the test's
  assembler output.

:samp:`scan-assembler-dem {regex} [{ target/xfail {selector} }]`
  Passes if :samp:`{regex}` matches text in the test's demangled assembler output.

:samp:`scan-assembler-dem-not {regex} [{ target/xfail {selector} }]`
  Passes if :samp:`{regex}` does not match text in the test's demangled assembler
  output.

:samp:`scan-assembler-symbol-section {functions}{section} [{ target/xfail {selector} }]`
  Passes if :samp:`{functions}` are all in :samp:`{section}`.  The caller needs to
  allow for ``USER_LABEL_PREFIX`` and different section name conventions.

:samp:`scan-symbol-section {filename}{functions}{section} [{ target/xfail {selector} }]`
  Passes if :samp:`{functions}` are all in :samp:`{section}` in :samp:`{filename}`.
  The same caveats as for ``scan-assembler-symbol-section`` apply.

:samp:`scan-hidden {symbol} [{ target/xfail {selector} }]`
  Passes if :samp:`{symbol}` is defined as a hidden symbol in the test's
  assembly output.

:samp:`scan-not-hidden {symbol} [{ target/xfail {selector} }]`
  Passes if :samp:`{symbol}` is not defined as a hidden symbol in the test's
  assembly output.

:samp:`check-function-bodies {prefix}{terminator} [{options} [{ target/xfail {selector} }]]`
  Looks through the source file for comments that give the expected assembly
  output for selected functions.  Each line of expected output starts with the
  prefix string :samp:`{prefix}` and the expected output for a function as a whole
  is followed by a line that starts with the string :samp:`{terminator}`.
  Specifying an empty terminator is equivalent to specifying :samp:`"*/"`.

  :samp:`{options}`, if specified, is a list of regular expressions, each of
  which matches a full command-line option.  A non-empty list prevents
  the test from running unless all of the given options are present on the
  command line.  This can help if a source file is compiled both with
  and without optimization, since it is rarely useful to check the full
  function body for unoptimized code.

  The first line of the expected output for a function :samp:`{fn}` has the form:

  .. code-block:: c++

    prefix fn:  [{ target/xfail selector }]

  Subsequent lines of the expected output also start with :samp:`{prefix}`.
  In both cases, whitespace after :samp:`{prefix}` is not significant.

  The test discards assembly directives such as ``.cfi_startproc``
  and local label definitions such as ``.LFB0`` from the compiler's
  assembly output.  It then matches the result against the expected
  output for a function as a single regular expression.  This means that
  later lines can use backslashes to refer back to :samp:`(...)`
  captures on earlier lines.  For example:

  .. code-block:: c++

    /* { dg-final { check-function-bodies "**" "" "-DCHECK_ASM" } } */
    ...
    /*
    ** add_w0_s8_m:
    **	mov	(z[0-9]+\.b), w0
    **	add	z0\.b, p0/m, z0\.b, \1
    **	ret
    */
    svint8_t add_w0_s8_m (...) { ... }
    ...
    /*
    ** add_b0_s8_m:
    **	mov	(z[0-9]+\.b), b0
    **	add	z1\.b, p0/m, z1\.b, \1
    **	ret
    */
    svint8_t add_b0_s8_m (...) { ... }

  checks whether the implementations of ``add_w0_s8_m`` and
  ``add_b0_s8_m`` match the regular expressions given.  The test only
  runs when :samp:`-DCHECK_ASM` is passed on the command line.

  It is possible to create non-capturing multi-line regular expression
  groups of the form :samp:`({a}|{b}|...)` by putting the
  :samp:`(`, :samp:`|` and :samp:`)` on separate lines (each still using
  :samp:`{prefix}`).  For example:

  .. code-block:: c++

    /*
    ** cmple_f16_tied:
    ** (
    **	fcmge	p0\.h, p0/z, z1\.h, z0\.h
    ** |
    **	fcmle	p0\.h, p0/z, z0\.h, z1\.h
    ** )
    **	ret
    */
    svbool_t cmple_f16_tied (...) { ... }

  checks whether ``cmple_f16_tied`` is implemented by the
  ``fcmge`` instruction followed by ``ret`` or by the
  ``fcmle`` instruction followed by ``ret``.  The test is
  still a single regular rexpression.

  A line containing just:

  .. code-block:: c++

    prefix ...

  stands for zero or more unmatched lines; the whitespace after
  :samp:`{prefix}` is again not significant.

Scan optimization dump files
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

These commands are available for :samp:`{kind}` of ``tree``, ``ltrans-tree``,
``offload-tree``, ``rtl``, ``offload-rtl``, ``ipa``, and
``wpa-ipa``.

:samp:`scan-{kind}-dump {regex}{suffix} [{ target/xfail {selector} }]`
  Passes if :samp:`{regex}` matches text in the dump file with suffix :samp:`{suffix}`.

:samp:`scan-{kind}-dump-not {regex}{suffix} [{ target/xfail {selector} }]`
  Passes if :samp:`{regex}` does not match text in the dump file with suffix
  :samp:`{suffix}`.

:samp:`scan-{kind}-dump-times {regex}{num}{suffix} [{ target/xfail {selector} }]`
  Passes if :samp:`{regex}` is found exactly :samp:`{num}` times in the dump file
  with suffix :samp:`{suffix}`.

:samp:`scan-{kind}-dump-dem {regex}{suffix} [{ target/xfail {selector} }]`
  Passes if :samp:`{regex}` matches demangled text in the dump file with
  suffix :samp:`{suffix}`.

:samp:`scan-{kind}-dump-dem-not {regex}{suffix} [{ target/xfail {selector} }]`
  Passes if :samp:`{regex}` does not match demangled text in the dump file with
  suffix :samp:`{suffix}`.

The :samp:`{suffix}` argument which describes the dump file to be scanned
may contain a glob pattern that must expand to exactly one file
name. This is useful if, e.g., different pass instances are executed
depending on torture testing command-line flags, producing dump files
whose names differ only in their pass instance number suffix.  For
example, to scan instances 1, 2, 3 of a tree pass 'mypass' for
occurrences of the string 'code has been optimized', use:

.. code-block:: c++

  /* { dg-options "-fdump-tree-mypass" } */
  /* { dg-final { scan-tree-dump "code has been optimized" "mypass\[1-3\]" } } */

Check for output files
~~~~~~~~~~~~~~~~~~~~~~

:samp:`output-exists [{ target/xfail {selector} }]`
  Passes if compiler output file exists.

:samp:`output-exists-not [{ target/xfail {selector} }]`
  Passes if compiler output file does not exist.

:samp:`scan-symbol {regexp} [{ target/xfail {selector} }]`
  Passes if the pattern is present in the final executable.

:samp:`scan-symbol-not {regexp} [{ target/xfail {selector} }]`
  Passes if the pattern is absent from the final executable.

Checks for gcov tests
~~~~~~~~~~~~~~~~~~~~~

:samp:`run-gcov {sourcefile}`
  Check line counts in :command:`gcov` tests.

:samp:`run-gcov [branches] [calls] { {opts}{sourcefile} }`
  Check branch and/or call counts, in addition to line counts, in
  :command:`gcov` tests.

:samp:`run-gcov-pytest { {sourcefile}{pytest_file} }`
  Check output of :command:`gcov` intermediate format with a pytest
  script.

Clean up generated test files
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Usually the test-framework removes files that were generated during
testing. If a testcase, for example, uses any dumping mechanism to
inspect a passes dump file, the testsuite recognized the dump option
passed to the tool and schedules a final cleanup to remove these files.

There are, however, following additional cleanup directives that can be
used to annotate a testcase "manually".

``cleanup-coverage-files``
  Removes coverage data files generated for this test.

:samp:`cleanup-modules "{list-of-extra-modules}"`
  Removes Fortran module files generated for this test, excluding the
  module names listed in keep-modules.
  Cleaning up module files is usually done automatically by the testsuite
  by looking at the source files and removing the modules after the test
  has been executed.

  .. code-block:: c++

    module MoD1
    end module MoD1
    module Mod2
    end module Mod2
    module moD3
    end module moD3
    module mod4
    end module mod4
    ! { dg-final { cleanup-modules "mod1 mod2" } } ! redundant
    ! { dg-final { keep-modules "mod3 mod4" } }

:samp:`keep-modules "{list-of-modules-not-to-delete}"`
  Whitespace separated list of module names that should not be deleted by
  cleanup-modules.
  If the list of modules is empty, all modules defined in this file are kept.

  .. code-block:: c++

    module maybe_unneeded
    end module maybe_unneeded
    module keep1
    end module keep1
    module keep2
    end module keep2
    ! { dg-final { keep-modules "keep1 keep2" } } ! just keep these two
    ! { dg-final { keep-modules "" } } ! keep all

:samp:`dg-keep-saved-temps "{list-of-suffixes-not-to-delete}"`
  Whitespace separated list of suffixes that should not be deleted
  automatically in a testcase that uses :option:`-save-temps`.

  .. code-block:: c++

    // { dg-options "-save-temps -fpch-preprocess -I." }
    int main() { return 0; }
    // { dg-keep-saved-temps ".s" } ! just keep assembler file
    // { dg-keep-saved-temps ".s" ".i" } ! ... and .i
    // { dg-keep-saved-temps ".ii" ".o" } ! or just .ii and .o

``cleanup-profile-file``
  Removes profiling files generated for this test.
