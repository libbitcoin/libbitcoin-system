#!/bin/bash
source ./bindings-env.sh

# Exit this script on the first error.
set -e

mkdir -p $TARGET_DIR/objs
mkdir -p $TARGET_DIR/lib
mkdir -p $TARGET_DIR/classes

gcc -fPIC -std=c++11 -c $GENERATED_SRC/src/main/cpp/java-wrap/$LIB_NAME.cpp -o $TARGET_DIR/objs/javawrap$LIB_NAME.o -I$JAVA_HOME/include -I$JAVA_HOME/include/linux
PACKAGE=chain && gcc -fPIC -std=c++11 -c $GENERATED_SRC/src/main/cpp/java-wrap/$LIB_NAME"_"$PACKAGE.cpp -o $TARGET_DIR/objs/javawrap$LIB_NAME"_"$PACKAGE.o -I$JAVA_HOME/include -I$JAVA_HOME/include/linux
exit
PACKAGE=config && gcc -fPIC -std=c++11 -c $GENERATED_SRC/src/main/cpp/java-wrap/$LIB_NAME"_"$PACKAGE.cpp -o $TARGET_DIR/objs/javawrap$LIB_NAME"_"$PACKAGE.o -I$JAVA_HOME/include -I$JAVA_HOME/include/linux
PACKAGE=formats && gcc -fPIC -std=c++11 -c $GENERATED_SRC/src/main/cpp/java-wrap/$LIB_NAME"_"$PACKAGE.cpp -o $TARGET_DIR/objs/javawrap$LIB_NAME"_"$PACKAGE.o -I$JAVA_HOME/include -I$JAVA_HOME/include/linux
PACKAGE=machine && gcc -fPIC -std=c++11 -c $GENERATED_SRC/src/main/cpp/java-wrap/$LIB_NAME"_"$PACKAGE.cpp -o $TARGET_DIR/objs/javawrap$LIB_NAME"_"$PACKAGE.o -I$JAVA_HOME/include -I$JAVA_HOME/include/linux
PACKAGE=math && gcc -fPIC -std=c++11 -c $GENERATED_SRC/src/main/cpp/java-wrap/$LIB_NAME"_"$PACKAGE.cpp -o $TARGET_DIR/objs/javawrap$LIB_NAME"_"$PACKAGE.o -I$JAVA_HOME/include -I$JAVA_HOME/include/linux
PACKAGE=message && gcc -fPIC -std=c++11 -c $GENERATED_SRC/src/main/cpp/java-wrap/$LIB_NAME"_"$PACKAGE.cpp -o $TARGET_DIR/objs/javawrap$LIB_NAME"_"$PACKAGE.o -I$JAVA_HOME/include -I$JAVA_HOME/include/linux
PACKAGE=utility && gcc -fPIC -std=c++11 -c $GENERATED_SRC/src/main/cpp/java-wrap/$LIB_NAME"_"$PACKAGE.cpp -o $TARGET_DIR/objs/javawrap$LIB_NAME"_"$PACKAGE.o -I$JAVA_HOME/include -I$JAVA_HOME/include/linux
PACKAGE=wallet && gcc -fPIC -std=c++11 -c $GENERATED_SRC/src/main/cpp/java-wrap/$LIB_NAME"_"$PACKAGE.cpp -o $TARGET_DIR/objs/javawrap$LIB_NAME"_"$PACKAGE.o -I$JAVA_HOME/include -I$JAVA_HOME/include/linux
gcc -fPIC  -shared \
	$TARGET_DIR/objs/javawrap$LIB_NAME.o \
	$TARGET_DIR/objs/javawrap$LIB_NAME_chain.o \
	$TARGET_DIR/objs/javawrap$LIB_NAME_config.o \
	$TARGET_DIR/objs/javawrap$LIB_NAME_formats.o \
	$TARGET_DIR/objs/javawrap$LIB_NAME_machine.o \
	$TARGET_DIR/objs/javawrap$LIB_NAME_math.o \
	$TARGET_DIR/objs/javawrap$LIB_NAME_message.o \
	$TARGET_DIR/objs/javawrap$LIB_NAME_utility.o \
	$TARGET_DIR/objs/javawrap$LIB_NAME_wallet.o \
	-o $TARGET_DIR/lib/libjavawrap$LIB_NAME.so -L/usr/local/lib -lbitcoin-$ARTIFACT_ID

#$JAVA_HOME/bin/javac -sourcepath $TARGET_DIR/generated-sources/src/main/java/ $TARGET_DIR/generated-sources/src/main/java/org/libbitcoin/$LIB_NAME/*.java -d $TARGET_DIR/classes
#$JAVA_HOME/bin/javac -sourcepath bindings/src/main/java/ bindings/src/main/java/org/libbitcoin/$LIB_NAME/*.java -d $TARGET_DIR/classes

#$JAVA_HOME/bin/jar cf $TARGET_DIR/lib/$LIB_NAME-$VERSION.jar -C $TARGET_DIR/classes .
#$JAVA_HOME/bin/jar uf $TARGET_DIR/lib/$LIB_NAME-$VERSION.jar -C $TARGET_DIR/lib libjavawrap$LIB_NAME.so 
