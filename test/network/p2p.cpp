/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
#include <cstdio>
#include <future>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;
using namespace bc::message;
using namespace bc::network;

#define TEST_SET_NAME \
    "network_tests"

#define TEST_NAME \
    boost::unit_test::framework::current_test_case().p_name

// TODO: build mock and/or use dedicated test service.
#define SEED1 "testnet-seed.bitcoin.petertodd.org:18333"
#define SEED2 "testnet-seed.bitcoin.schildbach.de:18333"

// NOTE: this is insufficient as the address varies.
#define SEED1_AUTHORITIES \
    { \
      { "52.8.185.53:18333" }, \
      { "178.21.118.174:18333" }, \
      { "[2604:880:d:2f::c7b2]:18333" }, \
      { "[2604:a880:1:20::269:b001]:18333" }, \
      { "[2602:ffea:1001:6ff::f922]:18333" }, \
      { "[2401:2500:203:9:153:120:11:18]:18333" }, \
      { "[2600:3c00::f03c:91ff:fe89:305f]:18333" }, \
      { "[2600:3c01::f03c:91ff:fe98:68bb]:18333" } \
    }

#define SETTINGS_TESTNET_ONE_THREAD_NO_CONNECTIONS(config) \
    settings config = p2p::testnet; \
    config.threads = 1; \
    config.host_pool_capacity = 0; \
    config.outbound_connections = 0; \
    config.connection_limit = 0

#define SETTINGS_TESTNET_ONE_THREAD_ONE_SEED(config) \
    SETTINGS_TESTNET_ONE_THREAD_NO_CONNECTIONS(config); \
    config.host_pool_capacity = 42; \
    config.seeds = { { SEED1 } }; \
    config.hosts_file = get_log_path(TEST_NAME, "hosts")

#define SETTINGS_TESTNET_TWO_THREADS_ONE_SEED_OUTBOUND(config) \
    settings config = p2p::testnet; \
    config.threads = 2; \
    config.connection_limit = 0; \
    config.seeds = { { SEED1 } }; \
    config.hosts_file = get_log_path(TEST_NAME, "hosts")

std::string get_log_path(const std::string& test, const std::string& file)
{
    const auto path = test + "." + file + ".log";
    boost::filesystem::remove_all(path);
    return path;
}

class log_setup_fixture
{
public:
    log_setup_fixture()
      : debug_log_(get_log_path(TEST_SET_NAME, "debug"), log_open_mode),
        error_log_(get_log_path(TEST_SET_NAME, "error"), log_open_mode)
    {
        initialize_logging(debug_log_, error_log_, std::cout, std::cerr);
    }

    ~log_setup_fixture()
    {
        log::clear();
    }

private:
    std::ofstream debug_log_;
    std::ofstream error_log_;
};

static void print_headers(const std::string& test)
{
    const auto header = "=========== " + test + " ==========";
    log::debug(TEST_SET_NAME) << header;
    log::error(TEST_SET_NAME) << header;
}

static int start_result(p2p& network)
{
    std::promise<code> promise;
    const auto handler = [&promise](code ec)
    {
        promise.set_value(ec);
    };
    network.start(handler);
    return promise.get_future().get().value();
}

static int stop_result(p2p& network)
{
    std::promise<code> promise;
    const auto handler = [&promise](code ec)
    {
        promise.set_value(ec);
    };
    network.stop(handler);
    return promise.get_future().get().value();
}

static int connect_result(p2p& network, const config::endpoint& host)
{
    std::promise<code> promise;
    const auto handler = [&promise](code ec, channel::ptr)
    {
        promise.set_value(ec);
    };
    network.connect(host.host(), host.port(), handler);
    return promise.get_future().get().value();
}

static int run_result(p2p& network)
{
    std::promise<code> promise;
    const auto handler = [&promise](code ec)
    {
        promise.set_value(ec);
    };
    network.run(handler);
    return promise.get_future().get().value();
}

static int subscribe_result(p2p& network)
{
    std::promise<code> promise;
    const auto handler = [&promise](code ec, channel::ptr)
    {
        promise.set_value(ec);
    };
    network.subscribe(handler);
    return promise.get_future().get().value();
}

static int subscribe_result(p2p& network, const config::endpoint& host)
{
    std::promise<code> promise;
    const auto handler = [&promise](code ec, channel::ptr)
    {
        promise.set_value(ec);
    };
    network.subscribe(handler);
    network.connect(host.host(), host.port());
    return promise.get_future().get().value();
}

template<class Message>
static int send_result(const Message& message, p2p& network, int channels)
{
    const auto channel_counter = [&channels](code ec, channel::ptr channel)
    {
        BOOST_REQUIRE_EQUAL(ec, error::success);
        --channels;
    };

    std::promise<code> promise;
    const auto completion_counter = [&promise](code ec)
    {
        promise.set_value(ec);
    };

    network.broadcast(message, channel_counter, completion_counter);
    const auto result = promise.get_future().get().value();

    BOOST_REQUIRE_EQUAL(channels, 0);
    return result;
}

BOOST_FIXTURE_TEST_SUITE(p2p_tests, log_setup_fixture)

BOOST_AUTO_TEST_CASE(p2p__height__default__zero)
{
    p2p network;
    BOOST_REQUIRE_EQUAL(network.height(), 0);
}

BOOST_AUTO_TEST_CASE(p2p__set_height__value__expected)
{
    p2p network;
    const size_t expected_height = 42;
    network.set_height(expected_height);
    BOOST_REQUIRE_EQUAL(network.height(), expected_height);
}

BOOST_AUTO_TEST_CASE(p2p__start__no_sessions__start_success)
{
    print_headers(TEST_NAME);
    SETTINGS_TESTNET_ONE_THREAD_NO_CONNECTIONS(configuration);
    p2p network(configuration);
    BOOST_REQUIRE_EQUAL(start_result(network), error::success);
}

BOOST_AUTO_TEST_CASE(p2p__start__no_connections__start_stop_success)
{
    print_headers(TEST_NAME);
    SETTINGS_TESTNET_ONE_THREAD_NO_CONNECTIONS(configuration);
    p2p network(configuration);
    BOOST_REQUIRE_EQUAL(start_result(network), error::success);
    BOOST_REQUIRE_EQUAL(stop_result(network), error::success);
}

BOOST_AUTO_TEST_CASE(p2p__start__no_sessions__start_success_start_operation_fail)
{
    print_headers(TEST_NAME);
    SETTINGS_TESTNET_ONE_THREAD_NO_CONNECTIONS(configuration);
    p2p network(configuration);
    BOOST_REQUIRE_EQUAL(start_result(network), error::success);
    BOOST_REQUIRE_EQUAL(start_result(network), error::operation_failed);
}

BOOST_AUTO_TEST_CASE(p2p__start__seed_session__start_stop_start_success)
{
    print_headers(TEST_NAME);
    SETTINGS_TESTNET_ONE_THREAD_ONE_SEED(configuration);
    p2p network(configuration);
    BOOST_REQUIRE_EQUAL(start_result(network), error::success);
    BOOST_REQUIRE_EQUAL(stop_result(network), error::success);
    BOOST_REQUIRE_EQUAL(start_result(network), error::success);
}

BOOST_AUTO_TEST_CASE(p2p__start__seed_session_handshake_timeout__start_operation_fail_stop_success)
{
    print_headers(TEST_NAME);
    SETTINGS_TESTNET_ONE_THREAD_ONE_SEED(configuration);
    configuration.channel_handshake_seconds = 0;
    p2p network(configuration);

    // The (timeout) error on the individual connection is ignored.
    // The connection failure results in a failure to generate any addresses.
    // The failure to germinate produces error::operation_failed.
    BOOST_REQUIRE_EQUAL(start_result(network), error::operation_failed);

    // The service never started but stop will still succeed (and is optional).
    BOOST_REQUIRE_EQUAL(stop_result(network), error::success);
}

BOOST_AUTO_TEST_CASE(p2p__start__seed_session_connect_timeout__start_operation_fail_stop_success)
{
    print_headers(TEST_NAME);
    SETTINGS_TESTNET_ONE_THREAD_ONE_SEED(configuration);
    configuration.connect_timeout_seconds = 0;
    p2p network(configuration);
    BOOST_REQUIRE_EQUAL(start_result(network), error::operation_failed);
    BOOST_REQUIRE_EQUAL(stop_result(network), error::success);
}

BOOST_AUTO_TEST_CASE(p2p__start__seed_session_germination_timeout__start_operation_fail_stop_success)
{
    print_headers(TEST_NAME);
    SETTINGS_TESTNET_ONE_THREAD_ONE_SEED(configuration);
    configuration.channel_germination_seconds = 0;
    p2p network(configuration);
    BOOST_REQUIRE_EQUAL(start_result(network), error::operation_failed);
    BOOST_REQUIRE_EQUAL(stop_result(network), error::success);
}

BOOST_AUTO_TEST_CASE(p2p__start__seed_session_inactivity_timeout__start_operation_fail_stop_success)
{
    print_headers(TEST_NAME);
    SETTINGS_TESTNET_ONE_THREAD_ONE_SEED(configuration);
    configuration.channel_inactivity_minutes = 0;
    p2p network(configuration);
    BOOST_REQUIRE_EQUAL(start_result(network), error::operation_failed);
    BOOST_REQUIRE_EQUAL(stop_result(network), error::success);
}

BOOST_AUTO_TEST_CASE(p2p__start__seed_session_expiration_timeout__start_operation_fail_stop_success)
{
    print_headers(TEST_NAME);
    SETTINGS_TESTNET_ONE_THREAD_ONE_SEED(configuration);
    configuration.channel_expiration_minutes = 0;
    p2p network(configuration);
    BOOST_REQUIRE_EQUAL(start_result(network), error::operation_failed);
    BOOST_REQUIRE_EQUAL(stop_result(network), error::success);
}

// Disabled for live test reliability.
// This may fail due to missing blacklist entries for the specified host.
////BOOST_AUTO_TEST_CASE(p2p__start__seed_session_blacklisted__start_operation_fail_stop_success)
////{
////    print_headers(TEST_NAME);
////    SETTINGS_TESTNET_ONE_THREAD_NO_CONNECTIONS(configuration);
////    configuration.host_pool_capacity = 42;
////    configuration.hosts_file = get_log_path(TEST_NAME, "hosts");
////    configuration.seeds = { { SEED1 } };
////    configuration.blacklists = SEED1_AUTHORITIES;
////    p2p network(configuration);
////    BOOST_REQUIRE_EQUAL(start_result(network), error::operation_failed);
////    BOOST_REQUIRE_EQUAL(stop_result(network), error::success);
////}

BOOST_AUTO_TEST_CASE(p2p__start__outbound_no_seeds__success)
{
    print_headers(TEST_NAME);
    SETTINGS_TESTNET_ONE_THREAD_NO_CONNECTIONS(configuration);
    configuration.outbound_connections = 1;
    p2p network(configuration);
    BOOST_REQUIRE_EQUAL(start_result(network), error::success);
}

BOOST_AUTO_TEST_CASE(p2p__connect__not_started__service_stopped)
{
    print_headers(TEST_NAME);
    SETTINGS_TESTNET_ONE_THREAD_NO_CONNECTIONS(configuration);
    p2p network(configuration);
    const config::endpoint host(SEED1);
    BOOST_REQUIRE_EQUAL(connect_result(network, host), error::service_stopped);
}

BOOST_AUTO_TEST_CASE(p2p__connect__started__success)
{
    print_headers(TEST_NAME);
    SETTINGS_TESTNET_ONE_THREAD_NO_CONNECTIONS(configuration);
    p2p network(configuration);
    const config::endpoint host(SEED1);
    BOOST_REQUIRE_EQUAL(start_result(network), error::success);
    BOOST_REQUIRE_EQUAL(run_result(network), error::success);
    BOOST_REQUIRE_EQUAL(connect_result(network, host), error::success);
}

// Disabled for live test reliability.
// This may fail due to connecting to the same host on different addresses.
////BOOST_AUTO_TEST_CASE(p2p__connect__twice__address_in_use)
////{
////    print_headers(TEST_NAME);
////    SETTINGS_TESTNET_ONE_THREAD_NO_CONNECTIONS(configuration);
////    p2p network(configuration);
////    const config::endpoint host(SEED1);
////    BOOST_REQUIRE_EQUAL(start_result(network), error::success);
////    BOOST_REQUIRE_EQUAL(run_result(network), error::success);
////    BOOST_REQUIRE_EQUAL(connect_result(network, host), error::success);
////    BOOST_REQUIRE_EQUAL(connect_result(network, host), error::address_in_use);
////}

BOOST_AUTO_TEST_CASE(p2p__subscribe__connect__success)
{
    print_headers(TEST_NAME);
    SETTINGS_TESTNET_ONE_THREAD_NO_CONNECTIONS(configuration);
    p2p network(configuration);
    const config::endpoint host(SEED1);
    BOOST_REQUIRE_EQUAL(start_result(network), error::success);
    BOOST_REQUIRE_EQUAL(run_result(network), error::success);
    BOOST_REQUIRE_EQUAL(subscribe_result(network, host), error::success);
}

BOOST_AUTO_TEST_CASE(p2p__subscribe__not_started__service_stopped)
{
    print_headers(TEST_NAME);
    SETTINGS_TESTNET_ONE_THREAD_NO_CONNECTIONS(configuration);
    p2p network(configuration);
    BOOST_REQUIRE_EQUAL(subscribe_result(network), error::service_stopped);
}

BOOST_AUTO_TEST_CASE(p2p__broadcast__ping_two_distinct_hosts__two_sends_and_successful_completion)
{
    print_headers(TEST_NAME);
    SETTINGS_TESTNET_ONE_THREAD_NO_CONNECTIONS(configuration);
    p2p network(configuration);
    const config::endpoint host1(SEED1);
    const config::endpoint host2(SEED2);
    BOOST_REQUIRE_EQUAL(start_result(network), error::success);
    BOOST_REQUIRE_EQUAL(run_result(network), error::success);
    BOOST_REQUIRE_EQUAL(connect_result(network, host1), error::success);
    BOOST_REQUIRE_EQUAL(connect_result(network, host2), error::success);
    BOOST_REQUIRE_EQUAL(send_result(ping(0), network, 2), error::success);
}

// This will hang for a long time if all connections are created before subscription.
BOOST_AUTO_TEST_CASE(p2p__subscribe__seed_outbound__success)
{
    print_headers(TEST_NAME);
    SETTINGS_TESTNET_TWO_THREADS_ONE_SEED_OUTBOUND(configuration);
    p2p network(configuration);
    BOOST_REQUIRE_EQUAL(start_result(network), error::success);
    BOOST_REQUIRE_EQUAL(run_result(network), error::success);
    BOOST_REQUIRE_EQUAL(subscribe_result(network), error::success);
}

BOOST_AUTO_TEST_SUITE_END()
