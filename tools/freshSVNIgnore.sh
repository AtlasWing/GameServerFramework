#!/bin/bash
# 重新设定.svnignore

find . -name ".svnignore" -exec sh -c 'svn propset svn:ignore -F "{}" `dirname "{}"`' \;
