import psycopg2

def cursor():
    db = psycopg2.connect(database='bitcoin', user='genjix', password='')
    return db.cursor()

