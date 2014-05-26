#!/bin/bash -e
echo "=== Creating sandboxes folder ==="
sudo rm -rf /sandboxes/
sudo mkdir /sandboxes/
sudo chown ubuntu:ubuntu /sandboxes/
mkdir /sandboxes/downloads/

echo "=== Building all games ==="
cd /Mjollnir/vigridr/src/
python build_all.py

echo "=== Setting up AWS auth ==="
echo "[Credentials]" > ~/.boto
echo "aws_access_key_id = $AWS_ACCESS_KEY_ID" >> ~/.boto
echo "aws_secret_access_key = $AWS_SECRET_ACCESS_KEY" >> ~/.boto

echo "=== Linking daemon file ==="
sudo ln -s /Mjollnir/yggdrasil/server/scripts/yggdrasil.init /etc/init.d/yggdrasil
