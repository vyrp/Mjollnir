#!/bin/bash -ex

cd ~
git clone https://github.com/git/git.git
cd git/contrib/subtree/
make
sudo install -m 755 git-subtree /usr/lib/git-core

if [ "$1" = "rm" ]; then
  cd ../../..
  rm -rf git
fi
