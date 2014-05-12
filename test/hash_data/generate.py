import hashlib
import random
import sys

def random_bytes():
    result = ""
    for i in range(random.randint(0, 50)):
        result += chr(random.randint(0, 255))
    return result

def generate(name, hashfunc):
    print "hash_result_list %s_tests{{" % name
    output_row = lambda bytes, result: \
        '    {"%s", "%s"}' % (bytes.encode("hex"), result)
    sys.stdout.write(output_row("", hashfunc("").hexdigest()))
    for i in range(20):
        print ","
        bytes = random_bytes()
        result = hashfunc(bytes).hexdigest()
        sys.stdout.write(output_row(bytes, result))
    print
    print "}};"
    print

def main():
    print "#include <string>"
    print "#include <vector>"
    print
    print "struct hash_result"
    print "{"
    print "    std::string input, result;"
    print "};"
    print
    print "typedef std::vector<hash_result> hash_result_list;"
    print
    generate("sha1", hashlib.sha1)

if __name__ == "__main__":
    main()

