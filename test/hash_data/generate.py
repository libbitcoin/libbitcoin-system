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
    sys.stdout.write(output_row("", hashfunc("")))
    for i in range(20):
        print ","
        bytes = random_bytes()
        result = hashfunc(bytes)
        sys.stdout.write(output_row(bytes, result))
    print
    print "}};"
    print

def sha1(data):
    return hashlib.sha1(data).hexdigest()

def sha256(data):
    return hashlib.sha256(data).hexdigest()

def ripemd(data):
    hasher = hashlib.new("ripemd160")
    hasher.update(data)
    return hasher.hexdigest()

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
    generate("sha1", sha1)
    generate("sha256", sha256)
    generate("ripemd", ripemd)

if __name__ == "__main__":
    main()

