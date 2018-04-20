# Do everything relative to this file location.
cd `dirname "$0"`
export BASE_DIR=`pwd`
export GROUP_ID=org.libbitcoin
export ARTIFACT_ID=bitcoin
export LIB_NAME=$ARTIFACT_ID
export MAJOR=1
export MINOR=-SNAPSHOT
export VERSION=$MAJOR$MINOR
export JAVA_HOME=/usr/lib/jvm/java-8-oracle/
#export BASE_DIR=$BITCOIN_DIR/libbitcoin-consensus
export TARGET_DIR=$BASE_DIR/bindings/target
export MAIN_SRC_DIR=$BASE_DIR/bindings/src/main
export TEST_SRC_DIR=$BASE_DIR/bindings/src/test
export GENERATED_SRC=$TARGET_DIR/generated-sources
