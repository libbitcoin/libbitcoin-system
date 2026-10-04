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
#ifndef LIBBITCOIN_SYSTEM_SRC_CRYPTO_SECP256K1_CUDA_TURNS_HPP
#define LIBBITCOIN_SYSTEM_SRC_CRYPTO_SECP256K1_CUDA_TURNS_HPP

#include <condition_variable>
#include <mutex>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace secp256k1 {
namespace cuda {

// Turns.
// ----------------------------------------------------------------------------

/// Device turns, by chunk, urgent callers ahead of queued callers.
class turns
{
public:
    void acquire(bool urgent) NOEXCEPT
    {
        std::unique_lock lock(mutex_);
        if (urgent)
        {
            ++urgent_;
            ready_.wait(lock, [&]() NOEXCEPT { return !busy_; });
            --urgent_;
        }
        else
        {
            const auto ticket = next_++;
            ready_.wait(lock, [&]() NOEXCEPT
            {
                return !busy_ && is_zero(urgent_) && serving_ == ticket;
            });

            ++serving_;
        }

        busy_ = true;
    }

    void release() NOEXCEPT
    {
        {
            std::lock_guard lock(mutex_);
            busy_ = false;
        }

        ready_.notify_all();
    }

    /// A caller is waiting that would be served ahead of the holder.
    bool contended(bool urgent) const NOEXCEPT
    {
        std::lock_guard lock(mutex_);
        return !is_zero(urgent_) || (!urgent && next_ != serving_);
    }

private:
    mutable std::mutex mutex_{};
    std::condition_variable ready_{};
    size_t urgent_{};
    uint64_t next_{};
    uint64_t serving_{};
    bool busy_{};
};

class turn
{
public:
    DELETE_COPY_MOVE(turn);

    turn(turns& turns, bool urgent) NOEXCEPT
      : turns_(turns), urgent_(urgent)
    {
        turns_.acquire(urgent_);
    }

    ~turn() NOEXCEPT
    {
        turns_.release();
    }

    bool contended() const NOEXCEPT
    {
        return turns_.contended(urgent_);
    }

    void yield() NOEXCEPT
    {
        turns_.release();
        turns_.acquire(urgent_);
    }

private:
    turns& turns_;
    const bool urgent_;
};

} // namespace cuda
} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
