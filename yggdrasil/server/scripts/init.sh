#!/bin/bash -e
echo "Creating sandboxes folder"
sudo rm -rf /sandboxes/
sudo mkdir /sandboxes/
sudo chown ubuntu:ubuntu /sandboxes/
mkdir /sandboxes/downloads/

echo "Building all"
cd /Mjollnir/vigridr/src/
python build_all.py
