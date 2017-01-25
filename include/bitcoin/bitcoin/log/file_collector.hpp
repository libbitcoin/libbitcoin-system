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

#ifndef LIBBITCOIN_LOG_FILE_COLLECTOR_HPP
#define LIBBITCOIN_LOG_FILE_COLLECTOR_HPP

#include <ctime>
#include <list>
#include <string>
#include <boost/enable_shared_from_this.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/list_hook.hpp>
#include <boost/intrusive/options.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/shared_ptr.hpp>

#if !defined(BOOST_LOG_NO_THREADS)
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#endif // !defined(BOOST_LOG_NO_THREADS)

#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {
namespace log {

// modified from class extracted from boost/log/sinks/text_file_backend.*pp
class BC_API file_collector_repository;

//! The string type compatible with the universal path type
typedef boost::filesystem::path::string_type path_string_type;

//! Type of the hook used for sequencing file collectors
typedef boost::intrusive::list_base_hook<boost::intrusive::link_mode<
    boost::intrusive::safe_link>> file_collector_hook;

class BC_API file_collector :
    public boost::log::sinks::file::collector,
    public file_collector_hook,
    public boost::enable_shared_from_this<file_collector>
{
public:
    file_collector(
        boost::shared_ptr<file_collector_repository> const& repo,
        boost::filesystem::path const& target_dir,
        size_t max_size,
        size_t min_free_space,
        size_t max_files);

    virtual ~file_collector();

    //! The function stores the specified file in the storage
    void store_file(boost::filesystem::path const& src_path) override;

    //! Scans the target directory for the files that have already been stored
    uintmax_t scan_for_files(boost::log::sinks::file::scan_method method,
        boost::filesystem::path const& pattern, unsigned int* counter) override;

    //! The function updates storage restrictions
    void update(size_t max_size, size_t min_free_space, size_t max_files);

    //! The function checks if the directory is governed by this collector
    bool is_governed(boost::filesystem::path const& dir) const;

private:
    //! Information about a single stored file
    struct file_info
    {
        uintmax_t size;
        std::time_t timestamp;
        boost::filesystem::path path;
    };
    //! A list of the stored files
    typedef std::list<file_info> file_list;

private:
    //! Makes relative path absolute with respect to the base path
    boost::filesystem::path make_absolute(boost::filesystem::path const& path);

    //! Acquires file name string from the path
    static path_string_type filename_string(
        boost::filesystem::path const& path);

private:
    //! A reference to the repository this collector belongs to
    boost::shared_ptr<file_collector_repository> repository_;

#if !defined(BOOST_LOG_NO_THREADS)
    //! Synchronization mutex
    boost::mutex mutex_;
#endif // !defined(BOOST_LOG_NO_THREADS)

    //! Total file size upper limit
    size_t max_size_;
    //! Free space lower limit
    size_t min_free_space_;
    //! File count upper limit
    size_t max_files_;

    //! The current path at the point when the collector is created
    /*
     * The special member is required to calculate absolute paths with no
     * dependency on the current path for the application, which may change
     */
    const boost::filesystem::path base_path_;
    //! Target directory to store files to
    boost::filesystem::path storage_dir_;

    //! The list of stored files
    file_list files_;
    //! Total size of the stored files
    uintmax_t total_size_;
};

} // namespace log
} // namespace libbitcoin

#endif
