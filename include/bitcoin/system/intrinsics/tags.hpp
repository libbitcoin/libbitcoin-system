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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_TAGS_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_TAGS_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/cpuid.hpp>
#include <bitcoin/system/intrinsics/detection.hpp>

namespace libbitcoin {
namespace system {

/// Release build tags. Each holds only where its extension and everything a
/// build with that tag compiles are present.
struct build_tags
{
    bool sse41{};
    bool avx2{};
    bool avx512{};
    bool avxifma{};
    bool avx512ifma{};
    bool shani{};
    bool sha2{};
    bool sha3{};
};

/// Instruction set extension support by the platform and by this build.
struct extension_state
{
    const char* name{};
    bool platform{};
    bool compiled{};
};

/// Tags supported by this processor.
template <auto Cpu = get_cpu, auto Xcr = get_xcr, auto Arm = get_arm>
BASELINE inline build_tags detected_tags() NOEXCEPT
{
    const auto aesni = try_aesni<Cpu>();

    build_tags tags{};
    tags.sse41 = try_sse41<Cpu>();
    tags.shani = try_shani<Cpu>() && try_x86_64_v2<Cpu>() && aesni;
    tags.avx2 = try_avx2<Cpu, Xcr>() && try_x86_64_v3<Cpu, Xcr>() && aesni;
    tags.avx512 = try_avx512<Cpu, Xcr>() && try_x86_64_v4<Cpu, Xcr>() && aesni;
    tags.avxifma = try_avxifma<Cpu, Xcr>() && tags.avx2 && tags.shani;
    tags.avx512ifma = try_avx512ifma<Cpu, Xcr>() && tags.avx512 && tags.shani;
    tags.sha2 = try_neon_sha2<Arm>() && try_neon_aes<Arm>();
    tags.sha3 = try_neon_sha3<Arm>() && tags.sha2 && try_lse<Arm>();
    return tags;
}

/// Tags compiled into this build.
constexpr build_tags compiled_tags() NOEXCEPT
{
    return
    {
        have_sse41,
        have_avx2,
        have_avx512,
        have_avxifma,
        have_avx512ifma,
        have_shani,
        have_neon_sha,
        have_neon_sha3
    };
}

/// Supported tags in canonical order, without those implied by another.
inline std::vector<std::string> build_suffix(const build_tags& tags) NOEXCEPT
{
    std::vector<std::string> out{};
    if (tags.avx512ifma)
    {
        out.emplace_back("avx512ifma");
    }
    else
    {
        if (tags.avx512)
            out.emplace_back("avx512");
        else if (tags.avx2 && !tags.avxifma)
            out.emplace_back("avx2");

        if (tags.avxifma)
            out.emplace_back("avxifma");
        else if (tags.shani)
            out.emplace_back("shani");
        else if (tags.sse41 && !tags.avx2 && !tags.avx512)
            out.emplace_back("sse41");
    }

    if (tags.sha3)
        out.emplace_back("sha3");
    else if (tags.sha2)
        out.emplace_back("sha2");

    return out;
}

/// Release file name of the program build for the tags on this platform, or
/// empty if there are no release builds for this platform.
inline std::string build_name(const std::string& program,
    const build_tags& tags) NOEXCEPT
{
#if defined(HAVE_LINUX)
    const std::string os{ "linux" };
#elif defined(HAVE_APPLE)
    const std::string os{ "macos" };
#elif defined(HAVE_WINDOWS)
    const std::string os{ "windows" };
#else
    const std::string os{};
#endif

#if defined(HAVE_X64)
    const std::string arch{ "x64" };
#elif defined(HAVE_ARM64)
    const std::string arch{ "arm64" };
#else
    const std::string arch{};
#endif

    if (os.empty() || arch.empty())
        return {};

    auto name = program + "-" + os + "-" + arch;
    for (const auto& tag: build_suffix(tags))
        name += "-" + tag;

#if defined(HAVE_WINDOWS)
    name += ".exe";
#endif

    return name;
}

/// Platform support and compilation of each instruction set extension.
inline std::vector<extension_state> extension_states() NOEXCEPT
{
#if defined(HAVE_ARM)
    return
    {
        { "aes",        try_neon_aes(),   have_neon_aes },
        { "sha2",       try_neon_sha2(),  have_neon_sha },
        { "sha3",       try_neon_sha3(),  have_neon_sha3 },
        { "neon",       try_neon(),       have_neon }
    };
#else
    return
    {
        { "aesni",      try_aesni(),      have_aesni },
        { "vaes",       try_vaes(),       have_vaes },
        { "shani",      try_shani(),      have_shani },
        { "sha512",     try_sha512(),     have_sha512 },
        { "sse41",      try_sse41(),      have_sse41 },
        { "avx2",       try_avx2(),       have_avx2 },
        { "avx512",     try_avx512(),     have_avx512 },
        { "avxifma",    try_avxifma(),    have_avxifma },
        { "avx512ifma", try_avx512ifma(), have_avx512ifma }
    };
#endif
}

} // namespace system
} // namespace libbitcoin

#endif
