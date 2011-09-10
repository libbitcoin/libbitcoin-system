#include "../src/storage/postgresql_blockchain.hpp"
#include <bitcoin/util/threaded_service.hpp>
#include <bitcoin/util/logger.hpp>
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
    void create_tree(std::string diagram);
    void save_nodes();
    void display_nodes();
    void raise_barrier();
    void delete_branch(std::string name);
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

void dummy_psql::create_tree(std::string diagram)
{
    boost::erase_all(diagram, " ");
    auto it = diagram.cbegin();
    if (*it != '(')
        return;
    node null;
    construct_branch(it, null);
}

void dummy_psql::save_nodes()
{
    sql_ << "TRUNCATE blocks" << cppdb::exec;
    sql_ << "SELECT setval('blocks_space_sequence', 1)" << cppdb::row;
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
            65534, \
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

int main()
{
    dummy_psql dum;
    dum.create_tree(diagram);
    dum.save_nodes();

    dum.raise_barrier();
    dum.raise_barrier();
    dum.raise_barrier();
    dum.raise_barrier();
    dum.raise_barrier();

    dum.delete_branch("bills");
    return 0;
}

