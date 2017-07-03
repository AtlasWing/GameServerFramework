#!/bin/sh

mkdir -p server/
cd server/
rm -rf release;
#rm -f *.tar.gz;
cd ..

echo "...创建目录"
mkdir -p server;
mkdir -p server/release;
mkdir -p server/release/zoneServer;
mkdir -p server/release/dataServer;
mkdir -p server/release/logicServer;
mkdir -p server/release/gatewayServer;

echo "...导出 sql..."
svn export sql server/release/sql;
echo "...导出 config..."
svn export config server/release/config;
echo "...导出 Script..."
svn export script server/release/script;
echo "...导出 tools..."
svn export tools server/release/tools;
echo "...导出libeutile.so..."
cp libbase_util.so server/release/;
echo "...导出zone..."
cp zoneServer/zoneServer server/release/zoneServer/;
echo "...导出data..."
cp dataServer/dataServer server/release/dataServer/;
echo "...导出logic..."
cp logicServer/logicServer server/release/logicServer/;
echo "...导出gateway..."
cp gatewayServer/gatewayServer server/release/gatewayServer/;
echo "...导出third_party..."
cp third_party/gperftools-2.1/lib/libprofiler.so.0 server/release/
cp third_party/gperftools-2.1/lib/libtcmalloc.so.4 server/release/
cp third_party/gperftools-2.1/lib/libunwind.so.7 server/release/
cp third_party/openssl-1.0.1/lib/*.so.* server/release/
cp third_party/curl-4.2.0/lib/libcares.so server/release/
cp third_party/curl-4.2.0/lib/libcurl.so.4 server/release/
svn info > server/release/version.txt;

echo "...数据压缩打包..."
cd server/
fileName="release_`date +%y_%m_%d_%H_%M_%S`.tar.gz"
tar cvzf "$fileName" release/;
cd ..;
echo "...版本发布完成";
