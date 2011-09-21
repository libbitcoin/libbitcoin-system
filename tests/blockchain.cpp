#include "../src/storage/postgresql_blockchain.hpp"
#include <bitcoin/util/threaded_service.hpp>
#include <bitcoin/util/logger.hpp>
#include <bitcoin/util/assert.hpp>
#include <bitcoin/storage/postgresql_storage.hpp>

#include <boost/algorithm/string/erase.hpp>

using namespace libbitcoin;

struct node
{
    std::string name, prev_name;
};

class override_delete
  : public postgresql_organizer
{
public:
    override_delete(cppdb::session sql)
      : postgresql_organizer(sql)
    {
    }
    void delete_branch(size_t space, size_t depth, 
        size_t span_left, size_t span_right)
    {
        postgresql_organizer::delete_branch(
            space, depth, span_left, span_right);
    }
};

// Tee hee 
class dummy_psql
  : public threaded_service
{
public:
    dummy_psql();
    void create_tree(std::string diagram, std::string prev_name);
    void reset_database();
    void save_nodes();
    void display_nodes();
    void raise_barrier();
    void delete_branch(std::string name);
    void fake_verify();
    void check_chains();
private:
    template<typename Iterator>
    void construct_branch(Iterator& it, const node& parent)
    {
        node n;
        n.prev_name = parent.name;
        while (true)
        {
            ++it;
            if (*it == '(' || *it == ')')
                break;
            else
                n.name += *it;
        }

        while (true)
        {
            if (*it == '(')
                construct_branch(it, n);
            else if (*it == ')')
                break;
            ++it;
        }
        // Add node to list
        nodes.push_back(n);
    }

    void display_branch(size_t depth, size_t left, size_t right);

    cppdb::session sql_;
    postgresql_blockchain_ptr blockchain_;
    shared_ptr<override_delete> deletor_;
    std::vector<node> nodes;
};

dummy_psql::dummy_psql()
  : sql_("postgresql:dbname=bitcoin;user=genjix")
{
    blockchain_.reset(new postgresql_blockchain(sql_, service()));
    deletor_.reset(new override_delete(sql_));
    blockchain_->set_clearance(4);
}

void dummy_psql::raise_barrier()
{
    blockchain_->raise_barrier();
}

void dummy_psql::create_tree(std::string diagram, std::string prev_name)
{
    boost::erase_all(diagram, " ");
    nodes.clear();
    auto it = diagram.cbegin();
    if (*it != '(')
        return;
    node null;
    null.name = prev_name;
    construct_branch(it, null);
}

void dummy_psql::reset_database()
{
    sql_ << "TRUNCATE blocks" << cppdb::exec;
    sql_ << "TRUNCATE chains" << cppdb::exec;
    sql_ <<
        "INSERT INTO chains ( \
            work, \
            chain_id, \
            depth \
        ) VALUES ( \
            difficulty(29, 65535), \
            0, \
            0 \
        )" << cppdb::exec;
    sql_ << "SELECT setval('blocks_space_sequence', 1)" << cppdb::row;
}
void dummy_psql::save_nodes()
{
    static cppdb::statement stat = sql_.prepare(
        "INSERT INTO blocks( \
            block_hash, \
            space, \
            depth, \
            span_left, \
            span_right, \
            version, \
            prev_block_hash, \
            merkle, \
            when_created, \
            bits_head, \
            bits_body, \
            nonce, \
            status \
        ) VALUES ( \
            ?, \
            nextval('blocks_space_sequence'), \
            0, \
            0, \
            0, \
            1, \
            ?, \
            'merkle', \
            TO_TIMESTAMP(1231006505), \
            29, \
            TRUNC(CASE ? WHEN 'root' THEN 1 ELSE RANDOM() END * 65535), \
            2083236893, \
            ? \
        )"
        );

    for (node n: nodes)
    {
        log_debug() << "n: " << n.prev_name << " " << n.name;
        stat.reset();
        stat.bind(n.name);
        stat.bind(n.prev_name);
        stat.bind(n.name);
        if (n.name == "root")
            stat.bind("verified");
        else
            stat.bind("orphan");
        stat.exec();
    }
    // Ugly but who cares... it's a test-case!
    sql_ << "UPDATE blocks SET space=0 WHERE block_hash='root'" << cppdb::exec;
}

void dummy_psql::display_nodes()
{
}

void dummy_psql::display_branch(size_t depth, size_t left, size_t right)
{
    static cppdb::statement stat = sql_.prepare(
        "SELECT block_hash \
        FROM blocks \
        WHERE \
            depth=? \
            AND span_left >= ? \
            AND span_right <= ?"
        );
    stat.reset();
    stat.bind(depth);
    stat.bind(left);
    stat.bind(right);
    cppdb::result r = stat.query();
    while (r.next())
    {
    }
}

void dummy_psql::delete_branch(std::string name)
{
    static cppdb::statement stat = sql_.prepare(
        "SELECT \
            space, \
            depth, \
            span_left, \
            span_right \
        FROM blocks \
        WHERE block_hash=?"
        );
    stat.reset();
    stat.bind(name);
    cppdb::result r = stat.row();
    if (r.empty())
    {
        log_error() << "Bad name for delete_branch()";
        return;
    }
    size_t space = r.get<size_t>(0), depth = r.get<size_t>(1), 
        span_left = r.get<size_t>(2), span_right = r.get<size_t>(3);
    log_debug() << "Deleting depth of " << depth
        << " [" << span_left << ", " << span_right << "]";

    deletor_->delete_branch(space, depth, span_left, span_right);
}

void dummy_psql::fake_verify()
{
    static cppdb::statement statement = sql_.prepare(
        "SELECT \
            *, \
            EXTRACT(EPOCH FROM when_created) timest \
        FROM blocks \
        WHERE \
            status='orphan' \
            AND space=0  \
        ORDER BY depth ASC"
        );
    statement.reset();
    cppdb::result result = statement.query();
    // foreach unverified block where status = orphan
    // do verification and set status = verified
    while (result.next())
    {
        sql_ <<
            "UPDATE chains \
            SET \
                work = work + difficulty(?, ?), \
                depth = ? \
            WHERE \
                chain_id >= ? \
                AND chain_id <= ?" 
            << result.get<size_t>("bits_head")
            << result.get<size_t>("bits_body")
            << result.get<size_t>("depth")
            << result.get<size_t>("span_left")
            << result.get<size_t>("span_right")
            << cppdb::exec;

        sql_ <<
            "UPDATE blocks \
            SET status='verified' \
            WHERE block_id=?"
            << result.get<size_t>("block_id") << cppdb::exec;
    }
}

void dummy_psql::check_chains()
{
    cppdb::result result = sql_ << "SELECT *, work * 10000 AS swork FROM chains";
    while (result.next())
    {
        size_t chain_id = result.get<size_t>("chain_id");
        size_t depth = result.get<size_t>("depth");
        cppdb::result verres = sql_ <<
            "SELECT \
                SUM(difficulty(bits_head, bits_body)) * 10000 AS work, \
                COUNT(bits_body) AS total \
            FROM blocks \
            WHERE \
                space=0 \
                AND depth <= ? \
                AND span_left <= ? \
                AND span_right >= ? \
                AND status='verified'"
            << depth
            << chain_id
            << chain_id
            << cppdb::row;
        BITCOIN_ASSERT(verres.get<size_t>("total") != 0);
        BITCOIN_ASSERT(verres.get<std::string>("work") == result.get<std::string>("swork"));
    }
}

const char* diagram = " \
(root \
    (toys \
        (ford \
            (blue) (boxxy) (caracas (venez)) \
        ) \
        (misagi \
            (nein \
                (yul) \
            ) \
        ) \
    ) \
    (bills \
        (zx81) \
        (electric \
            (gas \
                (oil \
                    (stat \
                        (sun) \
                    ) \
                    (water) \
                    (ice) \
                ) \
            ) \
        ) \
    ) \
    (sea) \
) \
";

const char* extended_diagram = "\
(hello (a) (b)) \
";

int main()
{
    dummy_psql dum;
    dum.create_tree(diagram, "");
    dum.reset_database();
    dum.save_nodes();
    dum.raise_barrier();
    dum.raise_barrier();
    dum.raise_barrier();
    dum.raise_barrier();
    dum.raise_barrier();
    dum.fake_verify();

    dum.create_tree(extended_diagram, "sea");
    dum.save_nodes();
    dum.raise_barrier();
    dum.raise_barrier();
    dum.raise_barrier();
    dum.raise_barrier();
    dum.raise_barrier();
    dum.fake_verify();

    dum.delete_branch("oil");
    dum.check_chains();

    return 0;
}

