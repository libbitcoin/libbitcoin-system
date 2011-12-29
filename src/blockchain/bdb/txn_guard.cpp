#include "txn_guard.hpp"

#include <bitcoin/util/assert.hpp>

namespace libbitcoin {

txn_guard::txn_guard(DbEnv* env)
 : used_(false)
{
    env->txn_begin(nullptr, &txn_, DB_READ_COMMITTED|DB_TXN_NOWAIT);
}
txn_guard::~txn_guard()
{
    if (!used_)
        txn_->abort();
    BITCOIN_ASSERT(used_);
}

void txn_guard::commit()
{
    BITCOIN_ASSERT(!used_);
    used_ = true;
    txn_->commit(0);
}
void txn_guard::abort()
{
    BITCOIN_ASSERT(!used_);
    used_ = true;
    txn_->abort();
}

DbTxn* txn_guard::get()
{
    return txn_;
}
const DbTxn* txn_guard::get() const
{
    return txn_;
}

} // libbitcoin

