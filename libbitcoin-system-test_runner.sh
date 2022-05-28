#!/bin/sh
###############################################################################
#  Copyright (c) 2014-2020 libbitcoin-system developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################

# Define tests and options.
#==============================================================================
BOOST_UNIT_TEST_OPTIONS=\
"--run_test=* "\
"--show_progress=no "\
"--detect_memory_leak=0 "\
"--report_level=no "\
"--build_info=yes"


# Run tests.
#==============================================================================
# ALlow CI to send errors to standard output
if [[ $CI == true ]]; then
    ./test/libbitcoin-system-test ${BOOST_UNIT_TEST_OPTIONS}
else
    ./test/libbitcoin-system-test ${BOOST_UNIT_TEST_OPTIONS} > test.log
fi
