#!/bin/bash
source ./bindings-env.sh

# Exit this script on the first error.
set -e

mkdir -p $TARGET_DIR/objs
mkdir -p $TARGET_DIR/lib
mkdir -p $TARGET_DIR/classes

export INCLUDE="-I$JAVA_HOME/include -I$JAVA_HOME/include/linux -I$MAIN_SRC_DIR/cpp -I$BASE_DIR/include"

#gcc -fPIC -std=c++11 -c $GENERATED_SRC/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID.cpp -o $TARGET_DIR/objs/javawrap$SUB_ARTIFACT_ID.o $INCLUDE
#PACKAGE=config && gcc -fPIC -std=c++11 -c $GENERATED_SRC/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID"_"$PACKAGE.cpp -o $TARGET_DIR/objs/javawrap$SUB_ARTIFACT_ID"_"$PACKAGE.o $INCLUDE
#PACKAGE=chain && gcc -fPIC -std=c++11 -c $GENERATED_SRC/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID"_"$PACKAGE.cpp -o $TARGET_DIR/objs/javawrap$SUB_ARTIFACT_ID"_"$PACKAGE.o $INCLUDE
#PACKAGE=formats && gcc -fPIC -std=c++11 -c $GENERATED_SRC/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID"_"$PACKAGE.cpp -o $TARGET_DIR/objs/javawrap$SUB_ARTIFACT_ID"_"$PACKAGE.o $INCLUDE
#PACKAGE=machine && gcc -fPIC -std=c++11 -c $GENERATED_SRC/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID"_"$PACKAGE.cpp -o $TARGET_DIR/objs/javawrap$SUB_ARTIFACT_ID"_"$PACKAGE.o $INCLUDE
#PACKAGE=math && gcc -fPIC -std=c++11 -c $GENERATED_SRC/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID"_"$PACKAGE.cpp -o $TARGET_DIR/objs/javawrap$SUB_ARTIFACT_ID"_"$PACKAGE.o $INCLUDE
#PACKAGE=message && gcc -fPIC -std=c++11 -c $GENERATED_SRC/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID"_"$PACKAGE.cpp -o $TARGET_DIR/objs/javawrap$SUB_ARTIFACT_ID"_"$PACKAGE.o $INCLUDE
PACKAGE=utility && gcc -fPIC -std=c++11 -c $GENERATED_SRC/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID"_"$PACKAGE.cpp -o $TARGET_DIR/objs/javawrap$SUB_ARTIFACT_ID"_"$PACKAGE.o $INCLUDE
#PACKAGE=wallet && gcc -fPIC -std=c++11 -c $GENERATED_SRC/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID"_"$PACKAGE.cpp -o $TARGET_DIR/objs/javawrap$SUB_ARTIFACT_ID"_"$PACKAGE.o $INCLUDE
exit
gcc -fPIC  -shared \
	$TARGET_DIR/objs/javawrap$SUB_ARTIFACT_ID.o \
	$TARGET_DIR/objs/javawrap$SUB_ARTIFACT_ID_chain.o \
	$TARGET_DIR/objs/javawrap$SUB_ARTIFACT_ID_config.o \
	$TARGET_DIR/objs/javawrap$SUB_ARTIFACT_ID_formats.o \
	$TARGET_DIR/objs/javawrap$SUB_ARTIFACT_ID_machine.o \
	$TARGET_DIR/objs/javawrap$SUB_ARTIFACT_ID_math.o \
	$TARGET_DIR/objs/javawrap$SUB_ARTIFACT_ID_message.o \
	$TARGET_DIR/objs/javawrap$SUB_ARTIFACT_ID_utility.o \
	$TARGET_DIR/objs/javawrap$SUB_ARTIFACT_ID_wallet.o \
	-o $TARGET_DIR/lib/libjavawrap$SUB_ARTIFACT_ID.so -L/usr/local/lib -lbitcoin-$ARTIFACT_ID

#$JAVA_HOME/bin/javac -sourcepath $TARGET_DIR/generated-sources/src/main/java/ $TARGET_DIR/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/*.java -d $TARGET_DIR/classes
#$JAVA_HOME/bin/javac -sourcepath bindings/src/main/java/ bindings/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/*.java -d $TARGET_DIR/classes

#$JAVA_HOME/bin/jar cf $TARGET_DIR/lib/$SUB_ARTIFACT_ID-$VERSION.jar -C $TARGET_DIR/classes .
#$JAVA_HOME/bin/jar uf $TARGET_DIR/lib/$SUB_ARTIFACT_ID-$VERSION.jar -C $TARGET_DIR/lib libjavawrap$SUB_ARTIFACT_ID.so 
