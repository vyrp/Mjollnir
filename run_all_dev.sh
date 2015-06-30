#!/bin/bash -ex

sudo mongod --fork --config /etc/mongodb.conf
cd /Mjollnir/bifrost
foreman start
