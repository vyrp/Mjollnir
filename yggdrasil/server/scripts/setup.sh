#!/bin/bash -e

echo "=== Creating sandboxes folder ==="
sudo rm -rf /sandboxes/
sudo mkdir /sandboxes/
sudo chown $(id -un):$(id -gn) /sandboxes/
mkdir /sandboxes/downloads/
mkdir /sandboxes/build/

echo "=== Building all games ==="
cd /Mjollnir/vigridr/src/
python build_all.py

echo "=== Setting up AWS auth ==="
echo "[Credentials]" > ~/.boto
echo "aws_access_key_id = $AWS_ACCESS_KEY_ID" >> ~/.boto
echo "aws_secret_access_key = $AWS_SECRET_ACCESS_KEY" >> ~/.boto

echo "=== Linking daemon files and logs folder ==="
sudo ln -s /Mjollnir/yggdrasil/server/scripts/yggdrasil.init /etc/init.d/yggdrasil
sudo ln -s /Mjollnir/jotunheim/jotunheim.init /etc/init.d/jotunheim
mkdir -p /Mjollnir/yggdrasil/server/logs
sudo update-rc.d yggdrasil defaults

echo "=== Pymongo ==="
sudo pip install pymongo==3.0.3
