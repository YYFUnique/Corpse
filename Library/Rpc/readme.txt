;生成idl文件
uuidgen.exe -i /oDeamon.idl
;编写对应的acf文件，编译时自动引用
RPC编译说明:
1. 使用midl Deamon.idl编译idl文件
2. RPC客户端需要包含Deamon_c.c文件,RPC服务端需要包含Deamon_s.c文件