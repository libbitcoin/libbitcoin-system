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
#ifndef LIBBITCOIN_SYSTEM_STREAM_MAKE_STREAM_HPP
#define LIBBITCOIN_SYSTEM_STREAM_MAKE_STREAM_HPP

#include <streambuf>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/device.hpp>

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

template <typename Device, typename Category>
constexpr bool is_device_category = std::is_base_of_v<Category,
    typename Device::category>;

/// std::streambuf over a device.
/// Direct input or output devices are buffered by their sequence.
/// Direct input/output devices share a single position, and are unbuffered.
/// Indirect devices are output only, buffered by optimal_buffer_size().
template <typename Device>
class device_buffer
  : public std::streambuf
{
public:
    DELETE_COPY_MOVE(device_buffer);

    static constexpr bool input = is_device_category<Device, ios::input>;
    static constexpr bool output = is_device_category<Device, ios::output>;
    static constexpr bool direct = is_device_category<Device, ios::direct_tag>;
    static constexpr bool flushable =
        is_device_category<Device, ios::flushable_tag>;

    static_assert(input || output);
    static_assert(direct || !input);

    template <typename... Args>
    device_buffer(Args&&... args) NOEXCEPT
      : device_(std::forward<Args>(args)...)
    {
        if constexpr (direct)
        {
            const auto sequence = input ? device_.input_sequence() :
                device_.output_sequence();

            begin_ = sequence.first;
            end_ = sequence.second;
            next_ = begin_;

            if constexpr (input && !output)
                setg(begin_, begin_, end_);
            else if constexpr (output && !input)
                setp(begin_, end_);
        }
        else
        {
            buffer_.resize(device_.optimal_buffer_size());
            reset_put();
        }
    }

    ~device_buffer() NOEXCEPT override
    {
        if constexpr (!direct)
        {
            try
            {
                flush_buffer();
            }
            catch (...)
            {
            }
        }
    }

protected:
    int_type underflow() override
    {
        if constexpr (input && output)
            return next_ == end_ ? traits_type::eof() :
                traits_type::to_int_type(*next_);
        else
            return traits_type::eof();
    }

    int_type uflow() override
    {
        if constexpr (input && output)
            return next_ == end_ ? traits_type::eof() :
                traits_type::to_int_type(*next_++);
        else
            return std::streambuf::uflow();
    }

    std::streamsize xsgetn(char_type* to, std::streamsize count) override
    {
        if constexpr (input && output)
        {
            const auto size = std::min<std::streamsize>(count, end_ - next_);
            std::copy_n(next_, size, to);
            next_ += size;
            return size;
        }
        else
        {
            return std::streambuf::xsgetn(to, count);
        }
    }

    std::streamsize xsputn(const char_type* from,
        std::streamsize count) override
    {
        if constexpr (input && output)
        {
            const auto size = std::min<std::streamsize>(count, end_ - next_);
            std::copy_n(from, size, next_);
            next_ += size;
            return size;
        }
        else
        {
            return std::streambuf::xsputn(from, count);
        }
    }

    int_type overflow(int_type character) override
    {
        if (traits_type::eq_int_type(character, traits_type::eof()))
            return flush_buffer() ? traits_type::not_eof(character) :
                traits_type::eof();

        if constexpr (input && output)
        {
            if (next_ == end_)
                return traits_type::eof();

            *next_++ = traits_type::to_char_type(character);
            return character;
        }
        else if constexpr (direct)
        {
            return traits_type::eof();
        }
        else
        {
            if (!flush_buffer())
                return traits_type::eof();

            const auto value = traits_type::to_char_type(character);
            if (buffer_.empty())
                return is_one(device_.write(&value, 1)) ? character :
                    traits_type::eof();

            *pptr() = value;
            pbump(1);
            return character;
        }
    }

    int sync() override
    {
        if (!flush_buffer())
            return -1;

        if constexpr (flushable)
            return device_.flush() ? 0 : -1;
        else
            return 0;
    }

    pos_type seekoff(off_type offset, std::ios_base::seekdir direction,
        std::ios_base::openmode mode) override
    {
        const auto failure = pos_type(off_type(-1));

        if constexpr (!direct)
        {
            return failure;
        }
        else
        {
            const auto in = is_nonzero(mode & std::ios_base::in);
            const auto out = is_nonzero(mode & std::ios_base::out);
            if ((in && !input) || (out && !output) || (!in && !out))
                return failure;

            off_type position{};
            if (direction == std::ios_base::cur)
                position = current() - begin_;
            else if (direction == std::ios_base::end)
                position = end_ - begin_;

            position += offset;
            if (is_negative(position) || position > end_ - begin_)
                return failure;

            const auto next = std::next(begin_, position);
            if constexpr (input && output)
                next_ = next;
            else if constexpr (input)
                setg(begin_, next, end_);
            else
                setp(next, end_);

            return pos_type(position);
        }
    }

    pos_type seekpos(pos_type position, std::ios_base::openmode mode) override
    {
        return seekoff(off_type(position), std::ios_base::beg, mode);
    }

private:
    char_type* current() const NOEXCEPT
    {
        if constexpr (input && output)
            return next_;
        else if constexpr (input)
            return gptr();
        else
            return pptr();
    }

    void reset_put() NOEXCEPT
    {
        const auto first = buffer_.data();
        setp(first, std::next(first, buffer_.size()));
    }

    bool flush_buffer()
    {
        if constexpr (direct)
        {
            return true;
        }
        else
        {
            const auto size = static_cast<std::streamsize>(pptr() - pbase());
            if (is_nonzero(size) && device_.write(pbase(), size) != size)
                return false;

            reset_put();
            return true;
        }
    }

    Device device_;
    std::vector<char_type> buffer_{};
    char_type* begin_{};
    char_type* end_{};
    char_type* next_{};
};

template <typename Device>
using device_stream = std::conditional_t<device_buffer<Device>::input,
    std::conditional_t<device_buffer<Device>::output, std::iostream,
        std::istream>, std::ostream>;

/// A std::istream, std::ostream or std::iostream (by category) over a device.
/// Constructor arguments are forwarded to the device constructor.
template <typename Device>
class make_stream
  : public device_stream<Device>
{
public:
    DELETE_COPY_MOVE(make_stream);

    template <typename... Args>
    make_stream(Args&&... args) NOEXCEPT
      : device_stream<Device>(nullptr),
        buffer_(std::forward<Args>(args)...)
    {
        this->rdbuf(&buffer_);
    }

private:
    device_buffer<Device> buffer_;
};

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#endif
