#!/bin/bash
# 转换编码及替换
fromKey="Hit";
toKey="MingZhong";
#find . -name "*.h" -exec sh -c 'iconv -f gb2312 -t utf-8 "{}" >"{}".bak;rm "{}";mv "{}".bak "{}";' \;
find ./logicServer/ -name "*.h" -exec sh -c 'cat "{}" | sed "s/$fromKey/$toKey/g" >"{}".bak;rm "{}";mv "{}".bak "{}";' \;
find ./logicServer/ -name "*.cpp" -exec sh -c 'cat "{}" | sed "s/$fromKey/$toKey/g" >"{}".bak;rm "{}";mv "{}".bak "{}";' \;
#find ./ -name "*.h" -exec sh -c ' svn mv "{}" `echo "{}" | sed 's/Logic/lg/g' ` ' \;
