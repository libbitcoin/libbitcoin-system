/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_UNIT_TEST_RUNNER_IPP
#define LIBBITCOIN_SYSTEM_UNIT_TEST_RUNNER_IPP

#include <cstdlib>

#if defined(HAVE_MSC) && defined(_DEBUG)
    #include <crtdbg.h>
#endif

namespace libbitcoin {
namespace system {
namespace unit_test {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

constexpr auto progress_scale =
    "0%   10   20   30   40   50   60   70   80   90   100%";
constexpr auto progress_ruler =
    "|----|----|----|----|----|----|----|----|----|----|";
constexpr auto report_indent = 2_size;
constexpr auto list_indent = 4_size;

inline runner::runner(const std::string& module) NOEXCEPT
  : module_(module), session_(session::instance())
{
}

inline int runner::run(int argc, const char* const argv[]) NOEXCEPT
{
    auto& settings = session_.settings();
    std::string error{};
    if (!settings.parse(error, argc, argv))
    {
        std::cerr << error << std::endl;
        return EXIT_FAILURE;
    }

    if (settings.help)
    {
        std::cout << "Options: --run_test=<filter>[:<filter>...] (or -t), "
            "--log_level=<level>, --report_level=no|confirm|short|detailed, "
            "--show_progress=yes|no, --build_info=yes|no, --list_content, "
            "--detect_memory_leaks=<number>, --help" << std::endl;
        return EXIT_SUCCESS;
    }

    configure_runtime();
    auto& master = registry::instance().master();
    path units{};
    selected_ = enable(master, units, { settings.run_test });

    if (settings.list_content)
    {
        list(master, zero);
        return EXIT_SUCCESS;
    }

    if (is_zero(selected_))
    {
        std::cerr << "Test setup error: no test cases matching filter or all "
            "test cases were disabled" << std::endl;
        return EXIT_FAILURE;
    }

    session_.log(log_level::error, "Running " + plural(selected_,
        "test case") + "...");

    if (settings.build_info)
        build_info();

    if (settings.show_progress)
        std::cout << "\n" << progress_scale << "\n" << progress_ruler <<
            std::endl;

    const auto start = clock::now();
    session_.log(log_level::test_suite, "Entering test module \"" + module_ +
        "\"");
    execute(master, units);
    session_.log(log_level::test_suite, "Leaving test module \"" + module_ +
        "\"; testing time: " + elapsed(start));

    if (settings.show_progress)
        std::cout << std::endl;

    report(master, "module", zero);
    const auto outcome = total(master);
    return is_zero(outcome.failed) ? EXIT_SUCCESS : EXIT_FAILURE;
}

// Selection.
// ----------------------------------------------------------------------------

inline size_t runner::enable(unit& suite, path& units,
    const filter& selection) NOEXCEPT
{
    size_t count{};
    for (auto& child: suite.children)
    {
        units.push_back(child.name);
        if (child.is_suite())
        {
            const auto enabled = enable(child, units, selection);
            child.enabled = is_nonzero(enabled);
            count += enabled;
        }
        else
        {
            child.enabled = selection.selects(units);
            count += child.enabled ? one : zero;
        }

        units.pop_back();
    }

    return count;
}

// Execution.
// ----------------------------------------------------------------------------

inline void runner::execute(unit& suite, path& units) NOEXCEPT
{
    for (auto& child: suite.children)
    {
        if (!child.enabled)
            continue;

        units.push_back(child.name);
        if (child.is_suite())
        {
            const auto start = clock::now();
            session_.log(log_level::test_suite, location(child) +
                "Entering test suite \"" + child.name + "\"");
            execute(child, units);
            session_.log(log_level::test_suite, location(child) +
                "Leaving test suite \"" + child.name + "\"; testing time: " +
                elapsed(start));
        }
        else
        {
            invoke(child, units);
        }

        units.pop_back();
    }
}

inline void runner::invoke(unit& test, const path& units) NOEXCEPT
{
    const auto start = clock::now();
    session_.start(test.name, join(units));
    session_.log(log_level::test_suite, location(test) +
        "Entering test case \"" + test.name + "\"");

    try
    {
        test.invoke();
    }
    catch (const aborted&)
    {
    }
    catch (const std::exception& exception)
    {
        session_.exception(std::string{ "std::exception: " } +
            exception.what());
    }
    catch (...)
    {
        session_.exception("unknown type");
    }

    test.result = session_.finish();
    session_.log(log_level::test_suite, location(test) +
        "Leaving test case \"" + test.name + "\"; testing time: " +
        elapsed(start));

    advance();
}

inline void runner::advance() NOEXCEPT
{
    ++completed_;
    if (!session_.settings().show_progress)
        return;

    const auto width = std::string_view{ progress_ruler }.size();
    const auto stars = (completed_ * width) / selected_;
    if (stars > stars_)
    {
        std::cout << std::string(stars - stars_, '*') << std::flush;
        stars_ = stars;
    }
}

// Reporting.
// ----------------------------------------------------------------------------

inline std::string runner::join(const path& units) NOEXCEPT
{
    std::string out{};
    for (const auto& name: units)
        out += (out.empty() ? "" : "/") + name;

    return out;
}

inline std::string runner::location(const unit& test) NOEXCEPT
{
#if defined(HAVE_MSC)
    return test.file + "(" + std::to_string(test.line) + "): ";
#else
    return test.file + ":" + std::to_string(test.line) + ": ";
#endif
}

inline std::string runner::elapsed(clock::time_point start) NOEXCEPT
{
    using namespace std::chrono;
    const auto span = duration_cast<microseconds>(clock::now() - start);
    return std::to_string(span.count()) + "us";
}

inline std::string runner::plural(size_t count,
    const std::string& noun) NOEXCEPT
{
    return std::to_string(count) + " " + noun + (count == one ? "" : "s");
}

inline runner::tally runner::total(const unit& suite) NOEXCEPT
{
    tally out{};
    for (const auto& child: suite.children)
    {
        if (!child.enabled)
            continue;

        if (child.is_suite())
        {
            const auto inner = total(child);
            out.cases += inner.cases;
            out.cases_passed += inner.cases_passed;
            out.cases_failed += inner.cases_failed;
            out.cases_aborted += inner.cases_aborted;
            out.passed += inner.passed;
            out.failed += inner.failed;
            out.warned += inner.warned;
        }
        else
        {
            const auto& result = child.result;
            ++out.cases;
            out.cases_passed += is_zero(result.failed) ? one : zero;
            out.cases_failed += is_zero(result.failed) ? zero : one;
            out.cases_aborted += result.aborted ? one : zero;
            out.passed += result.passed;
            out.failed += result.failed;
            out.warned += result.warned;
        }
    }

    return out;
}

inline void runner::report(const unit& suite, const std::string& kind,
    size_t depth) const NOEXCEPT
{
    const auto level = session_.settings().report;
    if (level == report_level::no)
        return;

    const auto outcome = total(suite);
    if (level == report_level::confirm)
    {
        if (is_zero(outcome.failed))
            std::cerr << "\n*** No errors detected" << std::endl;
        else
            std::cerr << "\n*** " << plural(outcome.failed, "failure") <<
                (outcome.failed == one ? " is" : " are") << " detected in "
                "the test module \"" << module_ << "\"" << std::endl;
        return;
    }

    const std::string indent(depth * report_indent, ' ');
    const auto name = is_zero(depth) ? module_ : suite.name;
    const auto passed = is_zero(outcome.failed);
    std::cerr << (is_zero(depth) ? "\n" : "") << indent << "Test " << kind <<
        " \"" << name << "\" has " << (passed ? "passed" : "failed") <<
        " with:\n";
    std::cerr << indent << "  " << plural(outcome.cases_passed, "test case")
        << " out of " << outcome.cases << " passed\n";
    if (is_nonzero(outcome.cases_failed))
        std::cerr << indent << "  " << plural(outcome.cases_failed,
            "test case") << " out of " << outcome.cases << " failed\n";
    if (is_nonzero(outcome.cases_aborted))
        std::cerr << indent << "  " << plural(outcome.cases_aborted,
            "test case") << " out of " << outcome.cases << " aborted\n";

    const auto assertions = outcome.passed + outcome.failed;
    std::cerr << indent << "  " << plural(outcome.passed, "assertion") <<
        " out of " << assertions << " passed\n";
    if (is_nonzero(outcome.failed))
        std::cerr << indent << "  " << plural(outcome.failed, "assertion") <<
            " out of " << assertions << " failed\n";
    if (is_nonzero(outcome.warned))
        std::cerr << indent << "  " << plural(outcome.warned, "warning") <<
            "\n";

    if (level != report_level::detailed)
        return;

    for (const auto& child: suite.children)
    {
        if (!child.enabled)
            continue;

        if (child.is_suite())
            report(child, "suite", add1(depth));
        else
            std::cerr << indent << "  Test case \"" << child.name <<
                "\" has " << (is_zero(child.result.failed) ? "passed" :
                "failed") << "\n";
    }
}

inline void runner::list(const unit& suite, size_t depth) const NOEXCEPT
{
    for (const auto& child: suite.children)
    {
        std::cerr << std::string(depth * list_indent, ' ') << child.name <<
            (child.enabled ? "*" : "") << "\n";

        if (child.is_suite())
            list(child, add1(depth));
    }
}

// Platform.
// ----------------------------------------------------------------------------

inline void runner::build_info() const NOEXCEPT
{
#if defined(HAVE_WINDOWS)
    const std::string platform{ "Windows" };
#elif defined(HAVE_APPLE)
    const std::string platform{ "macOS" };
#elif defined(HAVE_LINUX)
    const std::string platform{ "Linux" };
#elif defined(HAVE_FREEBSD)
    const std::string platform{ "FreeBSD" };
#else
    const std::string platform{ "unknown" };
#endif

#if defined(HAVE_CLANG)
    const std::string compiler{ "Clang version " __clang_version__ };
#elif defined(HAVE_GNUC)
    const std::string compiler{ "GNU C++ version " __VERSION__ };
#elif defined(HAVE_MSC)
    const auto compiler = "Microsoft Visual C++ version " +
        std::to_string(_MSC_FULL_VER);
#else
    const std::string compiler{ "unknown" };
#endif

#if defined(_MSVC_STL_VERSION)
    const auto library = "Microsoft STL version " +
        std::to_string(_MSVC_STL_VERSION);
#elif defined(_LIBCPP_VERSION)
    const auto library = "libc++ version " + std::to_string(_LIBCPP_VERSION);
#elif defined(__GLIBCXX__)
    const auto library = "GNU libstdc++ version " +
        std::to_string(__GLIBCXX__);
#else
    const std::string library{ "unknown" };
#endif

    std::cout << "Platform: " << platform << "\nCompiler: " << compiler <<
        "\nSTL     : " << library << std::endl;
}

// Debug runtime reports go to stderr (no dialogs), and leaks are reported at
// exit unless --detect_memory_leaks=0 (a greater value breaks on that
// allocation number).
inline void runner::configure_runtime() const NOEXCEPT
{
#if defined(HAVE_MSC) && defined(_DEBUG)
    for (const auto type: { _CRT_WARN, _CRT_ERROR, _CRT_ASSERT })
    {
        _CrtSetReportMode(type, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(type, _CRTDBG_FILE_STDERR);
    }

    const auto leaks = session_.settings().detect_memory_leaks;
    if (is_nonzero(leaks))
        _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) |
            _CRTDBG_LEAK_CHECK_DF);

    if (leaks > one)
        _CrtSetBreakAlloc(possible_narrow_sign_cast<long>(leaks));
#endif
}

BC_POP_WARNING()

} // namespace unit_test
} // namespace system
} // namespace libbitcoin

#endif
