### 注意点：
- 1.Eclipse，mysql,工作空间...目录不要含有中文

### 软件设置：
Eclipse
- 设置tomcat关联 window>proferences>Services>runtime environment
- 设置tomcat, server Locations (use tomcat installation) Deploy Path:webapps(项目发布路径)


### 注解：
注解和类，接口一样，是一种数据类型
可以用在类上，变量，方法上
可以有属性，也可以没有属性
有作用范围（源码，编译期间，运行期间）
源码期间有效的：String类上的@Author,@Since, @See (都是注释上的注解)
编译期间有效： @Override , @Deprecated, @Suppresswarning
运行期：@Test , 以JUnit形式运行


常见的注解：
@SuppressWarnings("unused")  // 防止编译警告
@Override  // 重写父类方法
@Test  JUnit4 第三方单元测试jar包
@SuppressWarnings({"unused", "rawtypes"})  // 消除rawtypes 泛型的编译警告
@Deprecated  // 声明过时的方法，不建议使用
@Test(timeout = 333) // 注解可以包含属性
@author // 注释里的注解，生成帮助文档的时候有用
@see // 注释里的注解，生成帮助文档的时候有用
```java
@Test(timeout=233)
void test() {
    try {
        Thread.sleep(19999);
    } catch(Exception e) {
        e.printStackTrace();
    }
    System.out.print("测试@Test注解")
}
// 会报错
```

自定义注解：
new Announcation
格式：
// 通过云注解@Retention说明当前自定义注解的作用域（Class,Source,Runtime）
@Retention(RetentionPolicy.RUNTIME) // 运行期间有效
// 通过元注解@Target，说名注解的目标对象
@Target(ElementType.METHOD)  // 说明注解用在哪里
public @interface 注解名称 {
    public 属性类型  属性名称1();
    public 属性类型  属性名称2() default 默认值;
}

属性类型可以包含：
only primitive type(基础类型) , String , class, annotation, enumeration , 1-dimensional arrays(以上类型的一维数组)

```java
public @interface MyAnno1 {
    // 定义属性
    public long timeout() default -1;
    // 
    public Class c() default java.util.Date.class;
    //
    public MyAnno2 ma2();
    //
    public String[] strs();
}
```

注解的作用：
配置作用，替代原来的xml, properties
Servlet2.x时，servlet还是配置在web.xml里
Servlet3.x时，可以使用@WebServlet注解

例子：
```java
// 1 将带有注解的类加载到内存中（Test.class字节码文件加载到内存中，class对象（代表字节码文件在内存中的对象））
Class tt = Class.forName("com.xxx.hw.Test");
或者
Class tt1 = new Test().getClass();
或者
Class tt2 = com.xxx.hw.Test.class;

// 2 获取字节码对象上所有方法
Method[] mds = tt.getMethods();
// 3 遍历方法对象，打印方法名称
for (Method m : mds) {
    System.out.println(m.getName());
    // 判断方法上是否存在注解
    if (md.isAnnotationPresent(MyAnno1.class)) {
        md.invoke(new Test(), args);
    }
}


```


### 设计模式
23种经典的设计模式
单例模式
工厂模式
装饰者模式
（二次开发的时候，无法使用继承，对已有的对象功能进行增强
前提：可以获取到被装饰对象实现的所有接口
实现思路：自定义装饰类实现接口，为自定义装饰类传递被装饰的对象
**弊端：如果被实现的接口中有很多的方法，那么装饰类里的方法就太冗余了------> 启用动态代理的思路**
）
动态代理
原理：通过代理调用底层API，来创建字节码文件（不用自己去写增强类了，简化了装饰者的创建） 通过虚拟机在内存中创建class文件
要创建的时候，要告诉虚拟机字节码文件上应该有多少方法



```java
// 获取被增强类的字节码文件上的所有接口
Class[] cls = GoogleCar.class.getInterfaces();
for (Class c : cls) {
    Method[] mds = c.getMethods();
}

// 通过代理创建增强类
import java.lang.reflect.Proxy;
//Proxy.newProxyInstance(loader, GoogleCar.class.getInterfaces(), h);

// 1param 固定值，告诉虚拟机用哪个字节码加载器加载内存中创建出的字节码文件
// 2param 告诉虚拟机内存中正在被创建的字节码文件中应该有哪些方法
// 3param 告诉虚拟机正在被创建的字节码上的各个方法如何来处理

// 创建代理对象
ICar car = (ICar) Proxy.newProxyInstance(Test.class.getClassLoader(), GoogleCar.class.getInterfaces(), new InvocationHandler() {
    // method: 代表正在执行中的方法
    // args : 代表正在执行中的方法的参数
    // Object : 代表方法执行完毕后的返回值
    @Override
    public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {

        // Object 代表方法执行完毕后的返回值
        Object obj = null;

        if (method.getName().equalsIgnoreCase("start")) {
            System.out.print("增强代码");
            obj = method.invoke(new GoogleCar(), args);
        } else {
            obj = method.invoke(new GoogleCar(), args);
        }

        return obj;
    }
});


```




适配器模式





### 类加载器
是负责加载类的对象，将class文件（硬盘）加载到内存生成class对象。
所有的类加载器都是java.lang.ClassLoader的子类。

1  BootstrapClassLoader 不是类
引导类加载器
（C, C++）
2  扩展类加载器
ExClassLoader (extension) 类

3  应用类加载器
AppClassLoader  类

使用：
类.class.getClassLoader()  // 获得自己的类加载器

类加载器加载机制：
全盘负责委托机制
全盘负责：A类如果要使用B类，则A类的加载器必须负责加载B类

```java
// 获取String.class的类加载器（引导类加载器）
ClassLoader c1 = String.class.getClassLoader();
System.out.println(c1);
// 由于String.class, Integer.class等字节码文件需要频繁的被加载到内存中，速度必须块，底层用其他语言来实现的 （C, C++）

// 获取ext(extention) 包下的某个类的类加载器。ExtClassLoader: 扩展类加载器
ClassLoader c1 = sun.net.spi.nameservie.dns.DNSNameService.class.getClassLoader();
System.out.println(c1);

// 应用类：程序员实现的所有类都属于应用类
// 应用类加载器
ClassLoader c1 = Test.class.getClassLoader();
System.out.println(c1);
```


Arrays.toString();


需要额外引入Servlet的jar包
javax.servlet-api-3.1.0.jar



### Dos命令
dir
md
rd
cd
cd..
cd/
del
exit
echo hello>1.txt

### 临时环境变量
set path = JAVA_HOME\bin
set path = JAVA_HOME%path%

set classpath = d:\112

八进制  0开头   0-7
十六进制  0x开头 0-9  A-F

四个二进制位就是一个十六进制位

0101-1010
5 - 10
0x5A

三个二进制位代表一个八进制位
001-011-010
1 - 3 - 2

负数的二进制变现形式：
对应的正数二进制取反

负数的最高位都是1

\n  换行
\t
\b
\r



<<: 乘以2移动的数次幂
>>  除以2移动的数次幂

3<<n   =  3 * 2^n
3>>n   =  3 / 2^n
3>>>1 = 1  3/2 = 1

>>> 无符号右移

>>: 最高位补什么由原有数据的最高位的值而定
如果最高位是0，右移后，用0补位
如果最高位是1，右移后，用1补位

>>>: 无论最高位是啥，都用0补位


swithc(表达式) {
    case 变量:
      // todo
      break;
    ...
    default:
        //todo
}
变量：byte,int,char,short,string
















