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

    postbind(strand_ptr strand, function memfunc)
      : strand_(strand), memfunc_(memfunc)
    {
    }

    void operator()(Args... params)
    {
        strand_->post(std::bind(memfunc_, std::forward<Args>(params)...));
    }
private:
    strand_ptr strand_;
    function memfunc_;
};

} // libbitcoin

#endif

