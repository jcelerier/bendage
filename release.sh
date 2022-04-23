#!/bin/bash
rm -rf release
mkdir -p release

cp -rf Bendage *.{hpp,cpp,txt,json} LICENSE release/

mv release score-addon-bendage
7z a score-addon-bendage.zip score-addon-bendage
