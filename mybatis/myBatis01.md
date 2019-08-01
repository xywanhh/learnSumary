## 持久层框架  

封装JDBC

ORM： Object Relation Mapping

建立对象和表的映射
操作对象就相当于操作表

Mybatis是ORM的一种实现






不需要
注册驱动
创建Connection
创建statement
手动设置参数
结果集检索

MyBatis通过xml或者注解的方式，将要执行的各种statement(statement, preparedStatement, CallableStatement) 配置起来，
并通过java对象和statement中的sql进行映射来生成最终的sql语句。
最有由mybatis框架执行sql，并将结果映射成java对象并返回。

![架构](/images/mybatis架构1.PNG)


1 SqlMappingConfig.xml

加入mybatis的核心包，依赖包，数据驱动包


2 Mapper.xml

