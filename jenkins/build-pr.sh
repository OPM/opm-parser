#!/bin/bash

source `dirname $0`/build-opm-parser.sh

ERT_REVISION=master
OPM_COMMON_REVISION=master
OPM_PARSER_REVISION=$sha1

if grep -q "ert=" <<< $ghprbCommentBody
then
  ERT_REVISION=pull/`echo $ghprbCommentBody | sed -r 's/.*ert=([0-9]+).*/\1/g'`/merge
fi
if grep -q "opm-common=" <<< $ghprbCommentBody
then
  OPM_COMMON_REVISION=pull/`echo $ghprbCommentBody | sed -r 's/.*opm-common=([0-9]+).*/\1/g'`/merge
fi

echo "Building with ert=$ERT_REVISION opm-common=$OPM_COMMON_REVISION opm-parser=$OPM_PARSER_REVISION"

build_opm_parser
test $? -eq 0 || exit 1

cp serial/build-opm-parser/testoutput.xml .
