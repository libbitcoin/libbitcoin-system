---------------------------------------------------------------------------
-- BASE TYPES
---------------------------------------------------------------------------
DROP DOMAIN IF EXISTS amount_type CASCADE;
CREATE DOMAIN amount_type AS NUMERIC(16, 8) CHECK (VALUE < 21000000 AND VALUE >= 0);
DROP DOMAIN IF EXISTS hash_type CASCADE;
CREATE DOMAIN hash_type AS VARCHAR(96);  -- 32*3 because "aa 0f ca ..."
DROP DOMAIN IF EXISTS address_type CASCADE;
CREATE DOMAIN address_type AS VARCHAR(110);

CREATE OR REPLACE FUNCTION internal_to_sql(value bigint) RETURNS amount_type AS $$
        BEGIN
                RETURN value / 100000000;
        END;
$$ LANGUAGE plpgsql;

---------------------------------------------------------------------------
-- BLOCKS
---------------------------------------------------------------------------

DROP TABLE IF EXISTS blocks;
DROP SEQUENCE IF EXISTS blocks_block_id_sequence;

CREATE SEQUENCE blocks_block_id_sequence;

CREATE TABLE blocks (
    block_id INT NOT NULL DEFAULT NEXTVAL('blocks_block_id_sequence') PRIMARY KEY,
    block_hash hash_type NOT NULL,
    depth INT,
    span_left INT,
    span_right INT,
    version BIGINT NOT NULL,
    prev_block_hash hash_type NOT NULL,
    merkle hash_type NOT NULL,
    when_created TIMESTAMP NOT NULL,
    bits_head INT NOT NULL,
    bits_body INT NOT NULL,
    nonce BIGINT NOT NULL,
    when_found TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);

---------------------------------------------------------------------------
-- INVENTORY QUEUE
---------------------------------------------------------------------------

DROP TABLE IF EXISTS inventory_requests;
DROP SEQUENCE IF EXISTS inventory_requests_inventory_id_sequence;
DROP TYPE IF EXISTS inventory_type;

CREATE SEQUENCE inventory_requests_inventory_id_sequence;

CREATE TYPE inventory_type AS ENUM ('block', 'transaction');

CREATE TABLE inventory_requests (
    inventory_id INT NOT NULL DEFAULT NEXTVAL('inventory_requests_inventory_id_sequence') PRIMARY KEY,
    type inventory_type NOT NULL,
    hash hash_type NOT NULL,
    when_discovered TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);

---------------------------------------------------------------------------
-- TRANSACTIONS
---------------------------------------------------------------------------

DROP TABLE IF EXISTS transactions;
DROP TABLE IF EXISTS transactions_parents;
DROP TABLE IF EXISTS outputs;
DROP TABLE IF EXISTS inputs;
DROP SEQUENCE IF EXISTS transactions_transaction_id_sequence;
DROP SEQUENCE IF EXISTS outputs_output_id_sequence;
DROP SEQUENCE IF EXISTS inputs_input_id_sequence;
DROP TYPE IF EXISTS output_transaction_type;

CREATE SEQUENCE transactions_transaction_id_sequence;
CREATE SEQUENCE outputs_output_id_sequence;
CREATE SEQUENCE inputs_input_id_sequence;

CREATE TYPE output_transaction_type AS ENUM ('normal', 'generate', 'other');

CREATE TABLE transactions_parents (
    transaction_id INT NOT NULL,
    block_id INT NOT NULL,
    index_in_block INT NOT NULL
);

CREATE TABLE transactions (
    transaction_id INT NOT NULL DEFAULT NEXTVAL('transactions_transaction_id_sequence') PRIMARY KEY,
    transaction_hash hash_type NOT NULL,
    version BIGINT NOT NULL,
    locktime BIGINT NOT NULL
);

CREATE TABLE outputs (
    output_id INT NOT NULL DEFAULT NEXTVAL('outputs_output_id_sequence') PRIMARY KEY,
    parent_id INT NOT NULL,
    index_in_parent INT NOT NULL,
    script_id INT NOT NULL,
    value amount_type NOT NULL,
    output_type output_transaction_type NOT NULL,
    address address_type
);

CREATE TABLE inputs (
    input_id INT NOT NULL DEFAULT NEXTVAL('inputs_input_id_sequence') PRIMARY KEY,
    parent_id INT NOT NULL,
    index_in_parent INT NOT NULL,
    script_id INT NOT NULL,
    previous_output_id INT,
    previous_output_hash hash_type NOT NULL,
    sequence BIGINT NOT NULL
);

---------------------------------------------------------------------------
-- SCRIPTS
---------------------------------------------------------------------------

-- use sequence for script_id

DROP TABLE IF EXISTS operations;
DROP SEQUENCE IF EXISTS operations_script_id_sequence;
DROP SEQUENCE IF EXISTS script_sequence;
DROP TYPE IF EXISTS opcode_type;
DROP TYPE IF EXISTS parent_ident_type;

CREATE SEQUENCE operations_script_id_sequence;
CREATE SEQUENCE script_sequence;

CREATE TYPE opcode_type AS ENUM (
    'special',
    'pushdata1',
    'pushdata2',
    'pushdata4',
    'dup',
    'hash160',
    'equalverify',
    'checksig'
);
CREATE TYPE parent_ident_type AS ENUM ('input', 'output');

CREATE TABLE operations (
    operation_id INT NOT NULL DEFAULT NEXTVAL('operations_script_id_sequence') PRIMARY KEY,
    opcode opcode_type NOT NULL,
    data varchar(255),
    script_id INT NOT NULL
);

