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

#ifndef LIBBITCOIN_SYSTEM_LOG_FILE_COLLECTOR_REPOSITORY_HPP
#define LIBBITCOIN_SYSTEM_LOG_FILE_COLLECTOR_REPOSITORY_HPP

#include <boost/intrusive/list.hpp>
#include <boost/intrusive/list_hook.hpp>
#include <boost/intrusive/options.hpp>
#include <boost/log/detail/singleton.hpp>

#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/log/file_collector.hpp>

namespace libbitcoin {
namespace log {

// modified from class extracted from boost/log/sinks/text_file_backend.*pp
class BC_API file_collector_repository :
    public boost::log::aux::lazy_singleton<file_collector_repository,
        boost::shared_ptr<file_collector_repository>>
{
public:
    //! Finds or creates a file collector
    boost::shared_ptr<boost::log::sinks::file::collector> get_collector(
        boost::filesystem::path const& target_dir, size_t max_size,
        size_t min_free_space, size_t max_files);

    //! Removes the file collector from the list
    void remove_collector(file_collector* collector);

private:
        //! Initializes the singleton instance
        static void init_instance();

private:
    //! Base type
    typedef boost::log::aux::lazy_singleton<file_collector_repository,
        boost::shared_ptr<file_collector_repository>> base_type;

#if !defined(BOOST_LOG_BROKEN_FRIEND_TEMPLATE_SPECIALIZATIONS)
    friend class boost::log::aux::lazy_singleton<file_collector_repository,
        boost::shared_ptr<file_collector_repository>>;
#else
    friend class base_type;
#endif

    //! The type of the list of collectors
    typedef boost::intrusive::list<
        file_collector,
        boost::intrusive::base_hook<file_collector_hook>> file_collectors;

private:
#if !defined(BOOST_LOG_NO_THREADS)
    //! Synchronization mutex
    boost::mutex mutex_;
#endif // !defined(BOOST_LOG_NO_THREADS)

    //! The list of file collectors
    file_collectors collectors_;
//    boost::intrusive::list<file_collector,
//        boost::intrusive::base_hook<file_collector_hook>> collectors_;
};

BC_API boost::shared_ptr<boost::log::sinks::file::collector> make_collector(
    boost::filesystem::path const& target_dir,
    size_t max_size,
    size_t min_free_space,
    size_t max_files = (std::numeric_limits<size_t>::max)()
);

} // namespace log
} // namespace libbitcoin

#endif
