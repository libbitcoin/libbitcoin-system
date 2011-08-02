#ifndef LIBBITCOIN_POSTBIND_H
#define LIBBITCOIN_POSTBIND_H

#include <functional>
#include <memory>

#include <bitcoin/types.hpp>

namespace libbitcoin {

template<typename... Args>
class postbind
{
public:
    typedef std::function<void (Args...)> function;

    postbind(service_ptr service, function memfunc)
      : service_(service), memfunc_(memfunc)
    {
    }

    void operator()(Args... params)
    {
        service_->post(std::bind(memfunc_, std::forward<Args>(params)...));
    }
private:
    service_ptr service_;
    function memfunc_;
};

} // libbitcoin

#endif

