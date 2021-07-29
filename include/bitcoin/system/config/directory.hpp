/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_DIRECTORY_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_DIRECTORY_HPP

#include <string>
#include <boost/filesystem.hpp>
#include <bitcoin/system/define.hpp>

// TODO: move to utf8_everwhere?

namespace libbitcoin {
namespace system {
namespace config {

// Declare config_default_path() via BC_DECLARE_CONFIG_DEFAULT_PATH(relative).
#define CONFIG_DEFAULT_PATH(directory, subdirectory) \
    static boost::filesystem::path config_default_path() \
    { \
        const boost::filesystem::path folder(directory); \
        return folder / subdirectory; \
    }

// The SYSCONFDIR symbol must be defined at compile for the project.
// Therefore this must be compiled directly into the relevant project(s).
#ifdef _MSC_VER
    #define BC_DECLARE_CONFIG_DEFAULT_PATH(relative) \
        CONFIG_DEFAULT_PATH(bc::system::config::windows_config_directory(),\
        relative)
#else
    #define BC_DECLARE_CONFIG_DEFAULT_PATH(relative) \
        CONFIG_DEFAULT_PATH(SYSCONFDIR, relative)
#endif

/**
 * Get the windows configuration directory.
 * @return Path or empty string if unable to retrieve.
 */
BC_API std::string windows_config_directory();

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
