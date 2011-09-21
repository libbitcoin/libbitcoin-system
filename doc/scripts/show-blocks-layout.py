import database
cursor = database.cursor()
# Assumes all blocks share the same space.
cursor.execute("""
    SELECT
        block_hash,
        depth,
        span_left,
        span_right
    FROM blocks
    WHERE space=0
    ORDER BY depth DESC
    """)
flat = []
tree = []
root = None
for name, depth, left, right in cursor.fetchall():
    node = [name, depth, left, right, []]
    is_child = lambda n: n[1] == depth + 1 and n[2] >= left and n[3] <= right
    node[-1].extend(filter(is_child, flat))
    flat.append(node)
    if name == 'root':
        root = node

def display_node(nodes, indent=0):
    for name, depth, left, right, children in nodes:
        print ' '*(indent*4)+name, '%i:%i'%(left, right)
        if children:
            display_node(children, indent+1)

display_node([root])

