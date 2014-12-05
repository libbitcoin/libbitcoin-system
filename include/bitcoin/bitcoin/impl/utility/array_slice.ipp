/*
 * Copyright (c) 2011-2014 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_ARRAY_SLICE_IPP
#define LIBBITCOIN_ARRAY_SLICE_IPP

namespace libbitcoin {

template<typename T>
template<typename Container>
array_slice<T>::array_slice(const Container& container)
  : begin_(container.data()), end_(container.data() + container.size())
{
}

template<typename T>
array_slice<T>::array_slice(const T* begin, const T* end)
  : begin_(begin), end_(end)
{
}

template<typename T>
const T* array_slice<T>::begin() const
{
    return begin_;
}

template<typename T>
const T* array_slice<T>::end() const
{
    return end_;
}

template<typename T>
const T* array_slice<T>::data() const
{
    return begin_;
}

template<typename T>
std::size_t array_slice<T>::size() const
{
    return end_ - begin_;
}

template<typename T>
bool array_slice<T>::empty() const
{
    return end_ == begin_;
}

} // namespace libbitcoin

#endif

