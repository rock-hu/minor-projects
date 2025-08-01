# 密钥生成与转换介绍

在以下场景中，经常需要使用密钥生成操作：

1. 随机生成算法库密钥对象。该对象可用于后续的加解密等操作。

2. 根据指定数据生成算法库密钥对象（也就是将外部或存储的二进制数据转换为算法库的密钥对象）。该对象可用于后续的加解密等操作。

3. 根据密钥参数生成指定的算法库密钥对象。该对象可用于后续的加解密等操作。

4. 获取算法库密钥对象的二进制数据，用于存储或传输。

5. 对于非对称密钥，获取密钥对象的参数属性，用于存储或传输。

其中，密钥对象Key包括对称密钥SymKey和非对称密钥（公钥PubKey和私钥PriKey），其中公钥和私钥组成密钥对KeyPair。
