import database
import sys

def output_is_final(output_id):
    cursor = database.cursor()
    cursor.execute("""
        SELECT transaction_hash
        FROM transactions, outputs
        WHERE
            transactions.transaction_id=outputs.output_id
            AND output_id=%s
        """, (output_id,))
    print cursor.fetchall()

if __name__ == '__main__':
    if len(sys.argv) < 2:
        output_id = 210123
    else:
        output_id = int(sys.argv[1])
    print output_is_final(output_id)

