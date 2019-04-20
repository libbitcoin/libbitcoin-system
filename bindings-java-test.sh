#!/bin/bash
source ./bindings-env.sh

rm -rf $TARGET_DIR/test-classes
mkdir -p $TARGET_DIR/test-classes

$JAVA_HOME/bin/javac -classpath $TARGET_DIR/lib/$LIB_NAME-$VERSION.jar -sourcepath $TEST_SRC_DIR/java $TEST_SRC_DIR/java/org/libbitcoin/$SUB_ARTIFACT_ID/*.java -d $TARGET_DIR/test-classes
$JAVA_HOME/bin/java -ea -Djava.library.path=$TARGET_DIR/lib -cp $TARGET_DIR/lib/$LIB_NAME-$VERSION.jar:$TARGET_DIR/test-classes $GROUP_ID.$LIB_NAME.$LIB_NAME"Test"
#$JAVA_HOME/bin/java -cp $TARGET_DIR/lib/$LIB_NAME-$VERSION.jar:$TARGET_DIR/test-classes $GROUP_ID.$LIB_NAME.$LIB_NAME"Test"
