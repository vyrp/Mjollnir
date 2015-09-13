#!/bin/bash
cd $(dirname $0)
python GameClient.py $* --thrift /Mjollnir/vigridr/third-parties/python/lib
