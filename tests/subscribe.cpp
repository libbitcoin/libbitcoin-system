#include <bitcoin/bitcoin.hpp>
using namespace bc;

void foo(const std::error_code& ec, const message::version& v)
{
    static size_t called = 0;
    log_debug() << "Weeeee #" << called;
}

typedef std::function<void (const std::error_code&, const message::version&)>
    example_handler_type;

typedef std::shared_ptr<subscriber<
    const std::error_code&, const message::version&>> subscribe_ver_ptr;

void defer_sub(subscribe_ver_ptr s, example_handler_type f)
{
    s->subscribe(f);
}

void callall(subscribe_ver_ptr s)
{
    s->relay(std::error_code(), message::version());
}

int main()
{
    thread_core_ptr thread = std::make_shared<thread_core>();
    strand_ptr str = thread->create_strand();
    auto sub = std::make_shared<subscriber<
        const std::error_code&, const message::version&>>(str);
    str->post(std::bind(defer_sub, sub, foo));
    str->post(std::bind(callall, sub));
    std::cin.get();
    return 0;
}

