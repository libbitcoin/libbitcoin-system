/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
package org.libbitcoin.bitcoin;

import java.util.logging.Logger;
import java.util.logging.Level;

class systemTest {
	private final static Logger logger = Logger.getLogger(systemTest.class.getName());
    private final static String TEST_FAILED = "Test Failed!";

    public static void assertTrue(boolean success) {
		if (!success) {
			logger.log(Level.SEVERE, TEST_FAILED, new RuntimeException(TEST_FAILED));
		}
	}

}

