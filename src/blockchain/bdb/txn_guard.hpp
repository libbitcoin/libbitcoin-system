#ifndef LIBBITCOIN_STORAGE_BERKELEYDB_TXN_GUARD_STORAGE_H
#define LIBBITCOIN_STORAGE_BERKELEYDB_TXN_GUARD_STORAGE_H

#include <memory>
#include DB_CXX_HEADER

namespace libbitcoin {

class txn_guard
{
public:
    txn_guard(DbEnv* env);
    ~txn_guard();

    txn_guard(const txn_guard&) = delete;
    void operator=(const txn_guard&) = delete;

    void commit();
    void abort();

    DbTxn* get();
    const DbTxn* get() const;

private:
    DbTxn* txn_;
    bool used_;
};

typedef std::shared_ptr<txn_guard> txn_guard_ptr;

} // libbitcoin

#endif

