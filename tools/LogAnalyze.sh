#!/bin/bash
#日志类型分类工具

#ERROR
grep -a "ERROR:" ~/log/*.log.$1-* |sed 's/.*ERROR://g'| sed 's/\[/</g'| sed 's/\]/>/g' | sed -r 's/\s*<[^>]*>\s*/ /g'| sort | uniq -c > logErrorType.$1

#INFO
grep -a "INFO:" ~/log/*.log.$1-* |sed 's/.*INFO://g'| sed 's/\[/</g'| sed 's/\]/>/g' | sed -r 's/\s*<[^>]*>\s*/ /g'| sort | uniq -c > logInfoType.$1

#DEBUG
grep -a "DEBUG:" ~/log/*.log.$1-* |sed 's/.*DEBUG://g'| sed 's/\[/</g'| sed 's/\]/>/g' | sed -r 's/\s*<[^>]*>\s*/ /g'| sort | uniq -c > logDebugType.$1

#TRACE
grep -a "TRACE:" ~/log/*.log.$1-* |sed 's/.*INFO://g'| sed 's/\[/</g'| sed 's/\]/>/g' | sed -r 's/\s*<[^>]*>\s*/ /g'| sort | uniq -c > logTraceType.$1
