/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SYSTEM_WARNINGS_HPP
#define LIBBITCOIN_SYSTEM_WARNINGS_HPP

#include <bitcoin/system/have.hpp>

#if defined(HAVE_PRAGMA_WARNING)
    #define BC_DISABLE_WARNING(value) \
        __pragma(warning(disable:value))
    #define BC_PUSH_WARNING(value) \
        __pragma(warning(push)) \
        __pragma(warning(disable:value))
    #define BC_POP_WARNING() \
        __pragma(warning(pop))
#else
    #define BC_PUSH_WARNING(value)
    #define BC_POP_WARNING()
#endif

#ifdef HAVE_MSC
    #define NARROWING_CONVERSION 4244
    #define DIAMOND_INHERITANCE 4250
    #define SIZE_NARROWING_CONVERSION 4267
    #define THROW_FROM_NOEXCEPT 4297
    #define TRUNCATED_CONSTANT 4310
    #define LOCAL_VARIABLE_NOT_INITIALIZED 4700

    // Global (see below).
    #define LOCAL_HIDES_GLOBAL 4459
    #define ASSIGNMENT_WITHIN_CONDITIONAL 4706

    // Lint.
    #define NO_NEW_DELETE 26409
    #define SMART_PTR_NOT_NEEDED 26415
    #define NO_RVALUE_REF_SHARED_PTR 26416
    #define NO_VALUE_OR_CONST_REF_SHARED_PTR 26418
    #define NO_GLOBAL_INIT_CALLS 26426
    #define USE_NOT_NULL 26429
    #define USE_GSL_AT 26446
    #define NO_THROW_IN_NOEXCEPT 26447
    #define NO_CASTS_FOR_ARITHMETIC_CONVERSION 26472
    #define NO_IDENTITY_CAST 26473
    #define NO_UNSAFE_COPY_N 26459
    #define NO_POINTER_ARITHMETIC 26481
    #define NO_DYNAMIC_ARRAY_INDEXING 26482
    #define NO_ARRAY_TO_POINTER_DECAY 26485
    #define NO_REINTERPRET_CAST 26490
    #define NO_CONST_CAST 26492
    #define USE_CONSTEXPR_FOR_FUNCTION 26497
#endif

#ifdef HAVE_MSC
    // Suppress C4706: assignment within conditional expression (we allow).
    BC_DISABLE_WARNING(ASSIGNMENT_WITHIN_CONDITIONAL)

    // Suppress C4459: declaration of 'one' hides global declaration.
    // This arises from boost templates  defining 'one' as a public variable.
    BC_DISABLE_WARNING(LOCAL_HIDES_GLOBAL)
#endif

#endif
