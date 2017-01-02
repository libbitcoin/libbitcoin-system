# Libbitcoin Style Guide

Maximum of 80 characters per line [exceptions allowed for test vectors], 4 space indents, no tabs.

Each continued line should be nested by 1 indent. Parameter stacking is discouraged.

If you're nesting more than 3 levels deep then generally you are doing something wrong. Reorganize and/or break it into functions. Always prefer flat over nested, dumb over clever, slow+clear over fast+obtuse.

Use `_` as opposed to `-` in source file names.

Variables, functions, `class`, `enum`, and `typedef` are lower case, `define` (macros) upper case, [CamelCase](http://en.wikipedia.org/wiki/CamelCase) identifiers for `template` types, no other mixed case.

Write self-documenting code. Names should be explanatory, not cryptic or ambiguous. Avoid abbreviations and acronyms.

Avoid [Hungarian notation](http://en.wikipedia.org/wiki/Hungarian_notation). Private class member variables are suffixed with `_`.

Functions should never be defined in headers unless required to be in every translation unit (i.e `template`, `inline` or `constexpr` functions).

It's nice if function names read like a sentence when used: `write_to_stream(data_packet);` or `say_hello_to(my_friend);`.

The `using` keyword should never be used in headers. Never declare `using namespace std`.

Every `{` begins a newline except for namespaces [there is some inconsistency here with respect to initializers].

Spaces between all operators: `x = 5 * 8 + 4 * 2`.

If the order is ambiguous or confusing then use parenthesis: `x = (5 * 8) + (4 * 2)`.

Avoid pre/postfix operators in compound expressions: `auto total = count++`.

Avoid raw pointers in C++, using smart pointers if pointers are necessary.

Use standard integer types where possible:

* Fixed-Length Unsigned Integers (`<cstdint>` / `<stdint.h>`)
    * `int8_t`
    * `int16_t`
    * `int32_t`
    * `int64_t`
* Fixed-Length Signed Integers (`<cstdint>` / `<stdint.h>`)
    * `uint8_t`
    * `uint16_t`
    * `uint32_t`
    * `uint64_t`
* Unspecified Length Unsigned Integer (`<cstddef>` / `<stddef.h>`)
    * `size_t`

Don't use `long` integers as this type varies in length by platform.

Don't assume `sizeof(void*) == sizeof(size_t)` as it is not guaranteed and it's semantically incorrect.

Generally you should use `size_t` for arbitrary length representation.

Don't treat non-boolean values or expressions as boolean, even in assertions, use comparison operators.

Use `char` for "characters", don't use `unsigned char` when you mean "byte", use `uint8_t`.

Assume `char` is interpreted as Utf-8, even in VC++! We have implemented "Utf-8 everywhere".

Use `boost::filesystem::path` for any part of a filesystem path, not `std::string`.

Each `:`, `;` (in for loops) and `,` has a space after it.

No spaces between function/macros name and left parenthesis: `do_bar(...)`. This greatly facilitates search.

Keywords such as `if` and `for`are not functions, one space before left parenthesis: `while (!done) ...`.

Flow control should transition on new lines.

```cpp
    // Bad
    if (foo) do_bar();

    // Good
    if (foo)
        do_bar();
```

Don't put more than one logical line on a line.

```cpp
    // Bad
    something(); another_thing(); yada_yada();

    // Good
    something(); 
    another_thing(); 
    yada_yada();
```

Be careful with the use of assertions, they get removed in `NDEBUG` builds and can cause breaks and unused variable/parameter warnings.

Use assertions **only** to facilitate debugging, **not** for error handling.

Guard all parameters of published API calls against bad data. Assersions are insufficient guards for API parameters.

Avoid direct use of numeric literals other than zero in a zero-based index. Use `sizeof()` if you are referring to a type size. Use `constexpr` or `const` for magic numbers.

Do not accept implicit narrowing casts. These are bugs that haven't been discovered (and produce warnings). Use `static_cast` but only if the conversion is first guarded.

Test subtraction for underflow and addition for overflow, before performing the operation.

Don't use global variables or `goto`, we are grown-ups now.

Don't tolerate warnings and don't suppress them due to laziness.

Use //// to temporarily disable code.

Use /// for public API documentation (short form) or javadoc format.

Use // for source code commentary.

Follow the closing of a scope with a blank line before a subsequent statement.

Precede comments with a blank line.

Define only one class or struct per file and match the file name to the object.

Use the following source file layouts.

### IPP Format

```cpp
    // license statement
    #ifndef LIBBITCOIN_FOO_IPP
    #define LIBBITCOIN_FOO_IPP

    // system includes (sorted)
    // other library includes (sorted)
    // bitcoin library includes (sorted)

    namespace libbitcoin {

    template <size_t Size>
    bool do_foo(bytes<Size>& out)
    {
        // ...
    }

    } // namespace libbitcoin

    #endif
```

### HPP Format

```cpp
    // license statement
    #ifndef LIBBITCOIN_FOO_HPP
    #define LIBBITCOIN_FOO_HPP

    // system includes (sorted)
    // other library includes (sorted)
    // bitcoin library includes (sorted)

    namespace libbitcoin {
    
    class foo
      : public bar
    {
    public:
        // typedefs
        // method declarations
        // member declarations

    private:
        // typedefs
        // method declarations
        // member declarations
    };

    } // namespace libbitcoin
    
    // function declarations
    // template includes (ipp) 

    #endif
```

### CPP Format

```cpp
    // license statement
    #include <bitcoin/foo.hpp>

    // system includes (sorted)
    // other library includes (sorted)
    // bitcoin library includes (sorted)

    namespace libbitcoin {

    // function implementations 
    // method implementations

    } // libbitcoin
```


### H Format

```c
    // license statement
    #ifndef LIBBITCOIN_BAR_H
    #define LIBBITCOIN_BAR_H

    // system includes (sorted)
    // other library includes (sorted)
    // bitcoin library includes (sorted)

#ifdef __cplusplus
extern "C" 
{
#endif

    // constant declarations
    // function declarations

#ifdef __cplusplus
}
#endif

#endif
```

### C Format

```c
    // license statement
    #include <bitcoin/bar.h>

    // system includes (sorted)
    // other library includes (sorted)
    // bitcoin library includes (sorted)
    
    // constant definitions
    // function implementations
```

### Test Considerations
*Tests have not been fully converted to this style in all repositories.*

It is important that tests can be evaluated visually. Otherwise it becomes impossible to determine if the test itself is correct. To achieve this result we apply certain style rules as follows:

#### Declarativeness.

This means that test execution is unconditional, there is no branching. Loops, if statements and the ternatory operator are prohibited. Test helpers may include conditional logic, but then these should be tested for correctness.

#### Transparent Names
When a test or set of tests fail this helps a developer to quickly isolate the cause. The naming convention is:
```c
BOOST_AUTO_TEST_CASE([class|file]__[method|function]__[condition]__[expectation])
```
For example:
```c
BOOST_AUTO_TEST_CASE(reservation__stopped__import_last_block__true)
```

Tests against a method, constructor or function override should use a simple ordinal numbering scheme to group test of the same signature. For example:
```c
BOOST_AUTO_TEST_CASE(configuration__construct1__none_context__expected)
BOOST_AUTO_TEST_CASE(configuration__construct1__mainnet_context__expected)
BOOST_AUTO_TEST_CASE(configuration__construct1__testnet_context__expected)
BOOST_AUTO_TEST_CASE(configuration__construct2__none_context__expected)
```

Group tests in a single `.cpp` file to mirror the source file naming convention. If it becomes necessary to break up the test file into independent files for a single corresponding class or source file, use the following test file naming convention:
```
class__method.cpp
file__function.cpp
```

Group tests semantically using `BOOST_AUTO_TEST_SUITE(...)`. This allows tests to be executed independently. These can be grouped hierarchically if necessary. Generally there is one suite per file or class under test.

#### Test One Thing

Test just one thing. The test name indicates the conditions and expectation of the test. When a test or set of tests fail this allows a developer to quickly isolate the cause. Sometimes it is convenient to test a set of results under one condition, such as a true result code and an expected out value. But these combinations should be semantically associated in that they are necessary to test together.

#### Line Length
A line of test code should not be wrapped. For this reason line length is not limited as in source files. However intermediate variables can and should be used to limit line length.

#### Test Helpers
The BOOST test helpers expose information about the test when there is a failure. It is preferred to use the least general test helper available. For example, use:

```c
BOOST_REQUIRE(!foo.is_valid())
```
instead of
```c
BOOST_REQUIRE_EQUAL(foo.is_valid(), false)
```
If values are serializable then use:
```c
BOOST_REQUIRE_EQUAL(foo.size(), 5u)
```
as opposed to
```c
BOOST_REQUIRE(foo.size() == 5u)
```
and in all cases placing the condition under test one the left side and the test expectation on the right.
