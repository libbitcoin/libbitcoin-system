#ifndef LIBBITCOIN_STORAGE_BERKELEYDB_TXN_GUARD_STORAGE_H
#define LIBBITCOIN_STORAGE_BERKELEYDB_TXN_GUARD_STORAGE_H

#include DB_CXX_HEADER

#include <bitcoin/util/assert.hpp>

namespace libbitcoin {

class txn_guard
{
public:
    txn_guard(DbEnv* env)
     : used_(false)
    {
        env->txn_begin(nullptr, &txn_, 0);
    }
    ~txn_guard()
    {
        if (!used_)
            txn_->abort();
        BITCOIN_ASSERT(used_);
    }

    txn_guard(const txn_guard&) = delete;
    void operator=(const txn_guard&) = delete;

    void commit()
    {
        BITCOIN_ASSERT(!used_);
        used_ = true;
        txn_->commit(0);
    }

    void abort()
    {
        BITCOIN_ASSERT(!used_);
        used_ = true;
        txn_->abort();
    }

    DbTxn* get()
    {
        return txn_;
    }

private:
    DbTxn* txn_;
    bool used_;
};

typedef std::shared_ptr<txn_guard> txn_guard_ptr;

} // libbitcoin

#endif

