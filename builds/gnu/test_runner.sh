#!/bin/sh
###############################################################################
#  Copyright (c) 2014-2026 libbitcoin-system-test developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################

# Define tests and options.
#==============================================================================
BOOST_UNIT_TEST_OPTIONS="--log_level=warning --show_progress=no --detect_memory_leak=0 --report_level=no --build_info=yes --run_test=*"

# Run tests.
#==============================================================================
if [[ ${CI} == true ]]; then
    ./test/libbitcoin-system-test ${BOOST_UNIT_TEST_OPTIONS}
else
    ./test/libbitcoin-system-test ${BOOST_UNIT_TEST_OPTIONS} > test.log
fi
