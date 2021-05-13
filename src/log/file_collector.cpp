/*
 *          Copyright Andrey Semashev 2007 - 2014.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   text_file_backend.cpp
 * \author Andrey Semashev
 * \date   09.06.2009
 *
 * \brief  This header is the Boost.Log library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/log/doc/html/index.html.
 */

// Modification of boost implementation to alter log file rotation naming.

#include <bitcoin/system/log/file_collector.hpp>

#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/spirit/home/qi/numeric/numeric_utils.hpp>
#include <bitcoin/system/log/file_char_traits.hpp>
#include <bitcoin/system/log/file_collector_repository.hpp>

namespace libbitcoin {
namespace system {
namespace log {

namespace filesystem = boost::filesystem;
namespace qi = boost::spirit::qi;

static boost::arg<1> _1;
static boost::arg<1> _2;

//! A possible Boost.Filesystem extension
//- renames or moves the file to the target storage
inline void move_file(
    filesystem::path const& from,
    filesystem::path const& to)
{
#ifdef BOOST_WINDOWS_API
    // On Windows MoveFile already does what we need
    filesystem::rename(from, to);
#else
    // On POSIX rename fails if the target points to a different device
    boost::system::error_code ec;
    filesystem::rename(from, to, ec);
    if (ec)
    {
        if (ec.value() == boost::system::errc::cross_device_link)
        {
            // Attempt to manually move the file instead
            filesystem::copy_file(from, to);
            filesystem::remove(from);
        }
        else
        {
            BOOST_THROW_EXCEPTION(filesystem::filesystem_error(
                "failed to move file to another location", from, to, ec));
        }
    }
#endif
}

//! The function parses the format placeholder for file counter
bool parse_counter_placeholder(path_string_type::const_iterator& it,
    path_string_type::const_iterator end, unsigned int& width)
{
    typedef qi::extract_uint<unsigned int, 10, 1, -1> width_extract;

    if (it == end)
        return false;

    path_string_type::value_type c = *it;
    if (c == file_char_traits::zero || c == file_char_traits::space ||
        c == file_char_traits::plus || c == file_char_traits::minus)
    {
        // Skip filler and alignment specification
        ++it;
        if (it == end)
            return false;
        c = *it;
    }

    if (file_char_traits::is_digit(c))
    {
        // Parse width
        if (!width_extract::call(it, end, width))
            return false;
        if (it == end)
            return false;
        c = *it;
    }

    if (c == file_char_traits::dot)
    {
        // Skip precision
        ++it;
        while (it != end && file_char_traits::is_digit(*it))
            ++it;
        if (it == end)
            return false;
        c = *it;
    }

    if (c == file_char_traits::number_placeholder)
    {
        ++it;
        return true;
    }

    return false;
}

//! The function matches the file name and the pattern
bool file_collector::match_pattern(path_string_type const& file_name,
    path_string_type const& pattern,
    unsigned int& file_counter)
{
    typedef qi::extract_uint<unsigned int, 10, 1, -1> file_counter_extract;

    struct local
    {
        // Verifies that the string contains exactly n digits
        static bool scan_digits(path_string_type::const_iterator& it,
            path_string_type::const_iterator end, std::ptrdiff_t n)
        {
            for (; n > 0; --n)
            {
                path_string_type::value_type c = *it++;
                if (!file_char_traits::is_digit(c) || it == end)
                    return false;
            }
            return true;
        }
    };

    path_string_type::const_iterator
        f_it = file_name.begin(),
        f_end = file_name.end(),
        p_it = pattern.begin(),
        p_end = pattern.end();
    bool placeholder_expected = false;
    while (f_it != f_end && p_it != p_end)
    {
        path_string_type::value_type p_c = *p_it,
            f_c = *f_it;

        if (!placeholder_expected)
        {
            if (p_c == file_char_traits::percent)
            {
                placeholder_expected = true;
                ++p_it;
            }
            else if (p_c == f_c)
            {
                ++p_it;
                ++f_it;
            }
            else
                return false;
        }
        else
        {
            switch (p_c)
            {
            case file_char_traits::percent: // An escaped '%'
                if (p_c == f_c)
                {
                    ++p_it;
                    ++f_it;
                    break;
                }
                else
                    return false;

            // Date/time components with 2-digits width
            case file_char_traits::seconds_placeholder:
            case file_char_traits::minutes_placeholder:
            case file_char_traits::hours_placeholder:
            case file_char_traits::day_placeholder:
            case file_char_traits::month_placeholder:
            case file_char_traits::year_placeholder:
                if (!local::scan_digits(f_it, f_end, 2))
                    return false;
                ++p_it;
                break;

            // Date/time components with 4-digits width
            case file_char_traits::full_year_placeholder:
                if (!local::scan_digits(f_it, f_end, 4))
                    return false;
                ++p_it;
                break;

            // Fraction seconds width is configuration-dependent
            case file_char_traits::frac_sec_placeholder:
                if (!local::scan_digits(f_it, f_end,
                    boost::posix_time::time_res_traits::num_fractional_digits()))
                {
                    return false;
                }
                ++p_it;
                break;

            // This should be the file counter placeholder or some unsupported placeholder
            default:
                {
                    path_string_type::const_iterator p = p_it;
                    unsigned int width = 0;
                    if (!parse_counter_placeholder(p, p_end, width))
                    {
                        BOOST_THROW_EXCEPTION(std::invalid_argument(
                            "Unsupported placeholder used in pattern for file scanning"));
                    }

                    // Ignore success of scan, use to burn potential
                    // single-character seperator used by formatter.
                    formatter_.scan_seperator(f_it, f_end);

                    // Find where the file number ends
                    path_string_type::const_iterator f = f_it;
                    if (!local::scan_digits(f, f_end, width))
                        return false;
                    while (f != f_end && file_char_traits::is_digit(*f))
                        ++f;

                    if (!file_counter_extract::call(f_it, f, file_counter))
                        return false;

                    p_it = p;
                }
                break;
            }

            placeholder_expected = false;
        }
    }

    if (p_it == p_end)
    {
        if (f_it != f_end)
        {
            // The actual file name may end with an additional counter
            // that is added by the collector in case if file name clash
            return formatter_.scan_seperator(f_it, f_end) &&
                local::scan_digits(f_it, f_end, std::distance(f_it, f_end));
        }
        else
            return true;
    }
    else
        return false;
}

file_collector::file_collector(
    boost::shared_ptr<file_collector_repository> const& repo,
    filesystem::path const& target_dir,
    size_t max_size,
    size_t min_free_space,
    size_t max_files)
  : repository_(repo), max_size_(max_size), min_free_space_(min_free_space),
    max_files_(max_files), base_path_(filesystem::current_path()),
    total_size_(0), formatter_(5)
{
    storage_dir_ = make_absolute(target_dir);
    filesystem::create_directories(storage_dir_);
}


file_collector::~file_collector()
{
    repository_->remove_collector(this);
}

//! The function stores the specified file in the storage
void file_collector::store_file(filesystem::path const& src_path)
{
    // NOTE FOR THE FOLLOWING CODE:
    // Avoid using Boost.Filesystem functions that would call path::codecvt(). store_file() can be called
    // at process termination, and the global codecvt facet can already be destroyed at this point.
    // https://svn.boost.org/trac/boost/ticket/8642

    // Let's construct the new file name
    file_info info;
    info.timestamp = filesystem::last_write_time(src_path);
    info.size = filesystem::file_size(src_path);

#ifdef _MSC_VER
    path_string_type stem = src_path.stem().wstring();
    path_string_type extension = src_path.extension().wstring();
#else
    path_string_type stem = src_path.stem().string();
    path_string_type extension = src_path.extension().string();
#endif

//    info.path = storage_dir_ / file_name_path;

    // If the file already exists, try to mangle the file name
    // to ensure there's no conflict. I'll need to make this customizable some day.
    unsigned int n = 0;
    do
    {
        filesystem::path alt_file_name = formatter_(stem, extension, n++);
        info.path = storage_dir_ / alt_file_name;
    }
    while (filesystem::exists(info.path) && n < (std::numeric_limits<unsigned int>::max)());


    // The directory should have been created in constructor, but just in case it got deleted since then...
    filesystem::create_directories(storage_dir_);

    BOOST_LOG_EXPR_IF_MT(boost::lock_guard<boost::mutex> lock(mutex_);)

    // Check if an old file should be erased
    uintmax_t free_space = min_free_space_ ?
        filesystem::space(storage_dir_).available : 0;

    file_list::iterator it = files_.begin(), end = files_.end();
    while (it != end &&
        (total_size_ + info.size > max_size_ || min_free_space_ > free_space || max_files_ <= files_.size()))
    {
        file_info& old_info = *it;
        if (filesystem::exists(old_info.path) && filesystem::is_regular_file(old_info.path))
        {
            try
            {
                filesystem::remove(old_info.path);
                // Free space has to be queried as it may not increase equally
                // to the erased file size on compressed filesystems
                if (min_free_space_)
                    free_space = filesystem::space(storage_dir_).available;
                total_size_ -= old_info.size;
                files_.erase(it++);
            }
            catch (boost::system::system_error&)
            {
                // Can't erase the file. Maybe it's locked? Never mind...
                ++it;
            }
        }
        else
        {
            // If it's not a file or is absent, just remove it from the list
            total_size_ -= old_info.size;
            files_.erase(it++);
        }
    }

    // Move/rename the file to the target storage
    move_file(src_path, info.path);

    files_.push_back(info);
    total_size_ += info.size;
}


//! Scans the target directory for the files that have already been stored
uintmax_t file_collector::scan_for_files(
    boost::log::sinks::file::scan_method method,
    filesystem::path const& pattern, unsigned int* counter)
{
    uintmax_t file_count = 0;
    if (method != boost::log::sinks::file::no_scan)
    {
        filesystem::path dir = storage_dir_;
        path_string_type mask;
        if (method == boost::log::sinks::file::scan_matching)
        {
            mask = filename_string(pattern);
            if (pattern.has_parent_path())
                dir = make_absolute(pattern.parent_path());
        }
        else
        {
            counter = NULL;
        }

        if (filesystem::exists(dir) && filesystem::is_directory(dir))
        {
            BOOST_LOG_EXPR_IF_MT(boost::lock_guard<boost::mutex> lock(mutex_);)

            if (counter)
                *counter = 0;

            file_list files;
            filesystem::directory_iterator it(dir), end;
            uintmax_t total_size = 0;
            for (; it != end; ++it)
            {
                file_info info;
                info.path = *it;
                if (filesystem::is_regular_file(info.path))
                {
                    // Check that there are no duplicates in the resulting list
                    struct local
                    {
                        static bool equivalent(filesystem::path const& left, file_info const& right)
                        {
                            return filesystem::equivalent(left, right.path);
                        }
                    };

                    if (std::find_if(files_.begin(), files_.end(),
                        boost::bind(&local::equivalent, boost::cref(info.path), _1)) == files_.end())
                    {
                        // Check that the file name matches the pattern
                        unsigned int file_number = 0;
                        if (method != boost::log::sinks::file::scan_matching ||
                            match_pattern(filename_string(info.path), mask, file_number))
                        {
                            info.size = filesystem::file_size(info.path);
                            total_size += info.size;
                            info.timestamp = filesystem::last_write_time(info.path);
                            files.push_back(info);
                            ++file_count;

                            if (counter && file_number >= *counter)
                                *counter = file_number + 1;
                        }
                    }
                }
            }

            // Sort files chronologically
            files_.splice(files_.end(), files);
            total_size_ += total_size;
            files_.sort(boost::bind(&file_info::timestamp, _1) < boost::bind(&file_info::timestamp, _2));
        }
    }

    return file_count;
}


//! The function updates storage restrictions
void file_collector::update(
    size_t max_size, size_t min_free_space, size_t max_files)
{
    BOOST_LOG_EXPR_IF_MT(boost::lock_guard<boost::mutex> lock(mutex_);)

    max_size_ = (std::min)(max_size_, max_size);
    min_free_space_ = (std::max)(min_free_space_, min_free_space);
    max_files_ = (std::min)(max_files_, max_files);
}

bool file_collector::is_governed(filesystem::path const& dir) const
{
    return filesystem::equivalent(storage_dir_, dir);
}

filesystem::path file_collector::make_absolute(
    filesystem::path const& path)
{
    return filesystem::absolute(path, base_path_);
}

path_string_type file_collector::filename_string(
    filesystem::path const& path)
{
    return path.filename().string<path_string_type>();
}

} // namespace log
} // namespace system
} // namespace libbitcoin
