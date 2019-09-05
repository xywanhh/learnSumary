java8带来的新特性--函数式编程
- Lambadas
- streams
- functional-style programming

如果你使用过SQL等数据库
查询语言，就会发现用几行代码写出的查询语句要是换成Java要写好长

行为参数化--> lambdas
流式处理，更大程度利用多核并行

:: 操作符 --> 把这个方法当做值进行传递，相当于创建一个方法引用

```java
// filter + collect
import static java.util.stream.Collections.toList;
List<Apple> subApples = 
    allApples.parallelStream().filter((Apple a) -> a.getWeight() > 100)
                              .collect(toList());

```

### 接口的默认方法
> list.stream()
> 早期的Collection接口中并没有提供stream方法，java8为了实现集合的stream()方法，提出了**接口的默认方法**实现



