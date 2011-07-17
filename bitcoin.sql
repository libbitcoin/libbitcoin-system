---------------------------------------------------------------------------
-- BASE TYPES
---------------------------------------------------------------------------
DROP DOMAIN IF EXISTS amount_type CASCADE;
CREATE DOMAIN amount_type AS NUMERIC(16, 8) CHECK (VALUE < 21000000 AND VALUE >= 0);

---------------------------------------------------------------------------
-- BLOCKS
---------------------------------------------------------------------------

DROP TABLE IF EXISTS blocks;
DROP SEQUENCE IF EXISTS blocks_block_id_sequence;
DROP TYPE IF EXISTS bits_type;

CREATE SEQUENCE blocks_block_id_sequence;

CREATE TYPE bits_type AS (sig INT, body INT);

CREATE TABLE blocks (
    block_id INT NOT NULL DEFAULT NEXTVAL('blocks_block_id_sequence') PRIMARY KEY,
    depth INT,
    span_left INT,
    span_right INT,
    version BIGINT NOT NULL,
    prev_block_hash VARCHAR(32) NOT NULL,
    merkle VARCHAR(32) NOT NULL,
    when_created TIMESTAMP NOT NULL,
    bits bits_type NOT NULL,
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
    hash VARCHAR(32) NOT NULL,
    when_discovered TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);

---------------------------------------------------------------------------
-- TRANSACTIONS
---------------------------------------------------------------------------

DROP TABLE IF EXISTS transactions;
DROP SEQUENCE IF EXISTS transactions_transaction_id_sequence;
DROP TYPE IF EXISTS transaction_type;

CREATE SEQUENCE transactions_transaction_id_sequence;

CREATE TYPE transaction_type AS ENUM ('normal', 'generate', 'other');

CREATE TABLE transactions (
    transaction_id INT NOT NULL DEFAULT NEXTVAL('transactions_transaction_id_sequence') PRIMARY KEY,
    parent_block_id INT,
    parent_block_sequence INT,
    amount amount_type,
    type transaction_type,
    is_final BOOL NOT NULL DEFAULT FALSE
);

---------------------------------------------------------------------------
-- SCRIPTS
---------------------------------------------------------------------------

-- use sequence for script_id

DROP TABLE IF EXISTS script_codes;
DROP SEQUENCE IF EXISTS script_codes_script_id_sequence;
DROP TYPE IF EXISTS opcode_type;

CREATE SEQUENCE script_codes_script_id_sequence;

CREATE TYPE opcode_type AS ENUM ('checksig', 'special', 'equalverify');

CREATE TABLE script_codes (
    operation_id INT NOT NULL DEFAULT NEXTVAL('script_codes_script_id_sequence') PRIMARY KEY,
    parent_transaction_id INT NOT NULL,
    opcode opcode_type NOT NULL,
    data varchar(200),
    script_id INT NOT NULL
);

