# secp256k1 椭圆曲线加密算法 #

1. 编译
>编译时增加参数：`./configure --enable-experimental --enable-module-ecdh --enable-module-recovery`  
>将recovery编译进去，直接make不会导出recovery

2. 参考资料
>`https://github.com/jakubtrnka/leightweightECDSA/blob/master/README.md`  
>`https://github.com/bitcoin-core/secp256k1`  
>`https://github.com/bitcoin-core/secp256k1/pull/490`  