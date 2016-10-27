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

#include <bitcoin/bitcoin/log/file_collector_repository.hpp>

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/smart_ptr/bad_weak_ptr.hpp>
#include <bitcoin/bitcoin/compat.hpp>

namespace libbitcoin {
namespace log {

static boost::arg<1> _1;

boost::shared_ptr<boost::log::sinks::file::collector> file_collector_repository::get_collector(
    boost::filesystem::path const& target_dir, size_t max_size,
    size_t min_free_space, size_t max_files)
{
    BOOST_LOG_EXPR_IF_MT(boost::lock_guard<boost::mutex> lock(mutex_);)

    file_collectors::iterator it = std::find_if(collectors_.begin(),
        collectors_.end(),
        boost::bind(&file_collector::is_governed, _1, boost::cref(target_dir)));

    boost::shared_ptr<file_collector> result;
    if (it != collectors_.end()) try
    {
        // This may throw if the collector is being currently destroyed
        result = it->shared_from_this();
        result->update(max_size, min_free_space, max_files);
    }
    catch (boost::bad_weak_ptr&)
    {
    }

    if (!result)
    {
        result = boost::make_shared<file_collector>(
            file_collector_repository::get(),
            target_dir, max_size, min_free_space, max_files);

        collectors_.push_back(*result);
    }

    return result;
}

void file_collector_repository::remove_collector(file_collector* collector)
{
    BOOST_LOG_EXPR_IF_MT(boost::lock_guard<boost::mutex> lock(mutex_);)
    collectors_.erase(collectors_.iterator_to(*collector));
}

void file_collector_repository::init_instance()
{
    base_type::get_instance() = boost::make_shared<file_collector_repository>();
}

//! Creates and returns a file collector with the specified parameters
boost::shared_ptr<boost::log::sinks::file::collector> make_collector(
    boost::filesystem::path const& target_dir,
    size_t max_size,
    size_t min_free_space,
    size_t max_files)
{
    return file_collector_repository::get()->get_collector(target_dir,
        max_size, min_free_space, max_files);
}

} // namespace log
} // namespace libbitcoin
