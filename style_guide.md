# Libbitcoin Style Guide

Maximum of 80 characters per line, 4 space indents, no tabs.

Each continued line should be nested by 1 indent.

If you're nesting more than 3 levels deep then generally you are doing something wrong. Reorganize and/or break it into functions. Always prefer flat over nested, dumb over clever, slow+clear over fast+obtuse.

Variables, functions, `class`, `enum`, and `typedef` are lower case, `define` (macros) upper case, [Camel Case](http://en.wikipedia.org/wiki/CamelCase) identifiers for `template` types, no other mixed case.

Avoid [Hungarian notation](http://en.wikipedia.org/wiki/Hungarian_notation).

Names should be explanatory, not cryptic or ambiguous.

Private class member variables are suffixed with `_`.

Functions should never be defined in headers unless required to be in every translation unit (i.e `template`, `inline` or `constexpr` functions).

It's nice if function names read like a sentence when used: `write_to_stream(data_packet);` or `say_hello_to(my_friend);`.

The `using` keyword should never be used in headers. Never declare `using namespace std`.

Every `{` begins a newline except for namespaces and initializers.

Spaces between all operators: `x = 5 * 8 + 4 * 2`.

If the order is ambiguous or confusing then use parenthesis: `x = (5 * 8) + (4 * 2)`.

Avoid per/postfix operators in compound expressions: `auto total = count++`.

Avoid the ternary operators: `auto letter = first ? alpha : omega`.

Avoid raw pointers, using smart pointers if necessary.

Each `:`, `;` (in for loops) and `,` has a space after it.

Be careful with the use of assertions, they get removed in `NDEBUG` builds and can cause breaks and unused variable/parameter warnings.

Don't tolerate warnings and don't suppress them due to laziness.

All flow control should happen on new lines.

### Flow Control

```cpp
    // Bad
    if (foo) do_bar();

    // Good
    if (foo)
        do_bar();

    // Bad
    something(); another_thing(); yada_yada();

    // Good
    something(); 
    another_thing(); 
    yada_yada();
```

### IPP Format

```cpp
    // license declaration
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
    // license declaration
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

      // functions logically grouped

      // variables

    private:
      // same as for public block
    };

    } // namespace libbitcoin
    
    // template (ipp) includes

    #endif
```

### CPP Format

```cpp
    // license declaration
    #include <bitcoin/foo.hpp>

    // system includes (sorted)
    // other library includes (sorted)
    // bitcoin library includes (sorted)

    namespace libbitcoin {

    } // libbitcoin
```
