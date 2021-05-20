/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/concurrency/dispatcher.hpp>

#include <memory>
#include <string>
#include <bitcoin/system/concurrency/threadpool.hpp>
#include <bitcoin/system/concurrency/work.hpp>

namespace libbitcoin {
namespace system {

dispatcher::dispatcher(threadpool& pool, const std::string& name)
  : heap_(std::make_shared<work>(pool, name)), pool_(pool)
{
}

////size_t dispatcher::ordered_backlog()
////{
////    return heap_->ordered_backlog();
////}
////
////size_t dispatcher::unordered_backlog()
////{
////    return heap_->unordered_backlog();
////}
////
////size_t dispatcher::concurrent_backlog()
////{
////    return heap_->concurrent_backlog();
////}
////
////size_t dispatcher::combined_backlog()
////{
////    return heap_->combined_backlog();
////}

} // namespace system
} // namespace libbitcoin
