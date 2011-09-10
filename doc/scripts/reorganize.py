import database

cursor = database.cursor()
cursor.execute("""
    SELECT
        block_id,
        prev_block_hash
    FROM blocks
    WHERE depth IS NULL
    ORDER BY block_id ASC
    """)
for block_id, prev_block_hash in cursor.fetchall():
    cursor.execute("""
        SELECT
            block_id,
            depth,
            span_left,
            span_right
        FROM blocks
        WHERE
            block_hash=%s
            AND depth IS NOT NULL
        """, (prev_block_hash,))
    parent_result = cursor.fetchone()
    if parent_result is None:
        continue
    parent_depth = parent_result[1] 
    parent_span_left = parent_result[2] 
    parent_span_right = parent_result[3]
    print 'Attaching', block_id

    cursor.execute("""
        SELECT 1
        FROM blocks
        WHERE
            span_left >= %s
            AND span_right <= %s
            AND depth > %s
        LIMIT 1
        """, (parent_span_left, parent_span_right, parent_depth))
    parent_has_children = (cursor.fetchone() is not None)

    depth = parent_depth + 1

    if parent_has_children:
        chain_id = parent_span_right
        cursor.execute("""
            UPDATE blocks
            SET span_left=span_left+1
            WHERE span_left >= %s
            """, (chain_id,))
        cursor.execute("""
            UPDATE blocks
            SET span_right=span_right+1
            WHERE span_right >= %s
            """, (chain_id,))
        db.commit()
    else:
        assert(parent_span_left == parent_span_right)
        chain_id = parent_span_left
        cursor.execute("""
            UPDATE blocks
            SET
                depth=%s,
                span_left=%s,
                span_right=%s
            WHERE block_id=%s
            """, (depth, chain_id, chain_id, block_id))
        db.commit()
        print depth, chain_id

