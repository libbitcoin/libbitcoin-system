/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/utility/work.hpp>

#include <memory>
#include <string>
#include <bitcoin/system/utility/delegates.hpp>
#include <bitcoin/system/utility/threadpool.hpp>

namespace libbitcoin {
namespace system {

work::work(threadpool& pool, const std::string& name)
  : name_(name),
    ////ordered_(std::make_shared<monitor::count>(0)),
    ////unordered_(std::make_shared<monitor::count>(0)),
    ////concurrent_(std::make_shared<monitor::count>(0)),
    ////sequential_(std::make_shared<monitor::count>(0)),
    service_(pool.service()),
    strand_(service_),
    sequence_(service_)
{
}

////size_t work::ordered_backlog()
////{
////    return ordered_->load();
////}
////
////size_t work::unordered_backlog()
////{
////    return unordered_->load();
////}
////
////size_t work::concurrent_backlog()
////{
////    return concurrent_->load();
////}
////
////size_t work::sequential_backlog()
////{
////    return sequential_->load();
////}
////
////size_t work::combined_backlog()
////{
////    return ordered_backlog() + unordered_backlog() + concurrent_backlog();
////}

} // namespace system
} // namespace libbitcoin
