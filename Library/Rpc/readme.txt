;生成idl文件
uuidgen.exe -i /oDeamon.idl
;编写对应的acf文件，编译时自动引用
RPC编译说明:
1、使用VS命令行提示工具，切换到Deamon.idl所在目录
2、使用midl Deamon.idl编译idl文件
3、如果没有问题，那么会生成Deamon.h,Deamon_c.c,Deamon_s.c三个文件，重新编译客户端和服务器端代码即可
4、RPC客户端需要包含Deamon_c.c文件,RPC服务端需要包含Deamon_s.c文件