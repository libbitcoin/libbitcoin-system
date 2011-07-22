---------------------------------------------------------------------------
-- BASE TYPES
---------------------------------------------------------------------------
DROP DOMAIN IF EXISTS amount_type CASCADE;
CREATE DOMAIN amount_type AS NUMERIC(16, 8) CHECK (VALUE < 21000000 AND VALUE >= 0);
DROP DOMAIN IF EXISTS hash_type CASCADE;
CREATE DOMAIN hash_type AS VARCHAR(95);  -- 32*3 because "aa 0f ca ..."
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

-- Genesis block
INSERT INTO blocks (
    block_hash,
    depth,
    span_left,
    span_right,
    version,
    prev_block_hash,
    merkle,
    when_created,
    bits_head,
    bits_body,
    nonce
) VALUES (
    '00 00 00 00 00 19 d6 68 9c 08 5a e1 65 83 1e 93 4f f7 63 ae 46 a2 a6 c1 72 b3 f1 b6 0a 8c e2 6f',
    0,
    0,
    0,
    1,
    '00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00',
    '4a 5e 1e 4b aa b8 9f 3a 32 51 8a 88 c3 1b c8 7f 61 8f 76 67 3e 2c c7 7a b2 12 7b 7a fd ed a3 3b',
    TO_TIMESTAMP(1231006505),
    29,
    65535,
    2083236893
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
    script_id INT NOT NULL,
    opcode opcode_type NOT NULL,
    data varchar(255)
);

