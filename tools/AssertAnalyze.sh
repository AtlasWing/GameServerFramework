#!/bin/bash
# assert分析工具

#查找路径

#vc Assert
grep -a "ASSERT FAIL:" ~/log/*.log.$1-* |sed 's/.*desc=//g'| sed 's/funcName.*//g' | sort |uniq -c > assertVc.log.$1
#lua Assert
grep -a "ASSERT.*脚本" ~/log/*.log.$1-* |sed 's/.*ASSERT.脚本//g'| sed 's/>.*//g' | sort |uniq -c > assertScript.log.$1



