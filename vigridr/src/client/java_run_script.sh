#!/bin/bash
cd $(dirname $0)
java -classpath ./client.jar:/MjollnirThirdParties/lib/* GameClient $*
