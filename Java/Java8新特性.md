## Jav8新特性

技术上将，新 < 稳定

提升：
速度更快
代码更少（新的语法 lambda）
强大的Stream  API
便于并行
最大化减少空指针 Optional


举例：
HashMap
初始大小16，加载因子0.75
在内存中的存储结构：数组 + 链表
每个对象都有equals 和 hashCode  
放入HashMap时，会根据对象的hashcode，采用hash算法算出一个索引值，放到数组里
然而hash算法并不能保证唯一，可能有不同的对象经过hash算法后，得到了相同的索引值，这个时候，1.7，后来的放到链表的头部，1.8后放到链表的尾部，
这种情况称为“碰撞”。
当插入的数据大小 > 表的大小 * 0.75后，会进行扩容（2倍）。
“碰撞”会降低性能，因为会遍历链表。
1.8后，当链表的长度>8 并且 map的大小 > 64，这种情况下，长度>8的链表转为红黑树，
红黑树带来的是查找，删除的性能的提升，增加元素的话，性能不如链表
1.8的存储结构：数组 + 链表 + 红黑树

HashSet同理

ConcurrentHashMap
1.7采用分段，每段有自己的并发级别，concurrentLevel = 16，默认是16，
分段不好分，分小了，浪费空间，分大了，里面的元素过多
1.8后，不采用分段，采用系统底层提供的CAS算法进行并发处理，性能提升


Hotspot
JRockit
J9
Taobao JVM

PremGen 永久区
现在JVM已经不把方法区放到永久区内了，方法区变为Metaspace（元空间）,采用物理内存




### Lambda表达式
是一个匿名函数，可以把表达式理解为一段可以传递的代码，更加简洁，灵活，也更紧凑


如何传递一段代码：
可以采用匿名内部类的方式：
```java
import  java.util.Comparator;
import  java.util.TreeSet;

import org.junit.Test;

@Test
public void test1() {
    Comparator<Integer> com = new Comparator<Integer>() {
        @Override
        public int compare(Integer o1, Integer o2) {
            return Integer.compare(o1, o2);
        }
    };
    TreeSet set = new TreeSet(com);
}

```
采用lambda表达式：
```java
@Test
public void test1() {
    Comparator<Integer> com = (x, y) -> Integer.compare(x, y);
    TreeSet set = new TreeSet(com);
}

```

eg:
接到一个需求，过滤age > 30
```java
class Person {
    int age;
    double salary;
}

常规写法:
List<Person> list1 = Arrays.asList(
    new Person(1, 1.1);
    new Person(1, 1.1);
    new Person(1, 1.1);
);

void filter1(List<Person> list) {
    List<Person> result = new ArrayList<Person>();
    for (Person p ：list1) {
        if (p.getAge > 1) {
            result.add(p);
        }
    }
}

如果再来一个需求： salary > 100
void filter2(List<Person> list) {
    List<Person> result = new ArrayList<Person>();
    for (Person p ：list1) {
        if (p.getSalary > 100) {
            result.add(p);
        }
    }
}


// 改进，采用策略模式
interface MyPredict<T> {
    boolean test(T t);
}

class MyPredictByAge implements MyPredict<Person> {
    @Override
    boolean test(Person p) {
        return p.getAge() > 1;
    }
}
class MyPredictBySalary implements MyPredict<Person> {
    @Override
    boolean test(Person p) {
        return p.getSalary() > 1;
    }
}

filter3(List<Person> list, MyPredict<Person> my) {
    List<Person> result = new ArrayList<Person>();
    for (Person p ：list1) {
        if (my.test()) {
            result.add(p);
        }
    }
}
MyPredictByAge my1 = new MyPredictByAge();
MyPredictBySalary my2 = new MyPredictBySalary();
filter31(list, my1);
filter31(list, my2);
或

// 匿名内部类
filter3(List<Person> list, new MyPredict<Person>() {
    @Override
    public boolean test(Person p) {
        return p.getAge > 100;
    }
});

// 改用lambda
filter(List<Person> list, (p) -> p.getAge() > 100);
result.forEach(System.out::println);

// 继续改进 Stream API
lists.stream()
     .filter((e) -> e.getAge() > 100)
     .limit(2)
     .forEach(System.out::println);

lists.stream()
     .map(Person::getName)
     .sorted()
     .forEach(System.out::println);

```

Lambda新的语法格式：

-> 箭头操作符
左边： 表达式的参数
右边： 表达式的主体（对函数式接口抽象方法的实现）

左右遇一括号省
左侧推断类型省
能省则省

需要函数式接口的支持

语法格式1 ： 无参数，无返回值
```java
() -> System.out.println("hello");

eg: 
@Test
void test() {
    int num = 0; // 默认是final int num = 0;
    Runnable r1 = new Runnable() {
        @Override
        public void run() {
            System.out.println("hello" + num); // 匿名内部类调用同级别的变量时，变量必须是final修饰
        }
    };
    r1.run();

    Runnable r2 = () -> System.out.println("hello1"); // lambda是语法糖，本质上还是一个方法
    r2.run();
}
```

语法格式2 ： 有一个参数，无返回值
```
(x) ->  System.out.println("hello");

eg:
@Test
void test() {
    Consumer<String> con = (x) -> System.out.println(x);
    con.accept("hello");

}

```

语法格式3 ： 若只有一个参数，括号可以不写
```java
x ->  System.out.println("hello");

eg:
@Test
void test() {
    Consumer<String> con = x -> System.out.println(x);
    con.accept("hello");

}

```


语法格式4 ： 有多个参数，有个返回值
```java
(x, y) -> {

}

eg:
@Test
void test() {
    Comparator<Integer> con = (x, y) -> {
        System.out.println("hello");
        return Integer.compare(x, y);
    };

}

```


语法格式5 ： 若表达式只有一条语句，return和{} 都可以省略不写
```java
(x, y) -> xxxxxx;

eg:
@Test
void test() {
    Comparator<Integer> con = (x, y) -> Integer.compare(x, y);

}

```

语法格式6 ： 参数类型可以省略，JVM编译期通过上下文推断出数据类型，叫“类型推断”， List<String> l = new ArrayList<>(); 
```java
(x, y) -> xxxxxx;
或
(Integer x, Integer y) -> xxxxxx;
eg:
@Test
void test() {
    Comparator<Integer> con = (x, y) -> Integer.compare(x, y);

}

```



“类型推断”： 1.8以上没问题，1.7有问题， 也是一种语法糖
1 List<String> l = new ArrayList<>(); 
2 
void test(Map<Integer, String> map) {}
test(new HashMap<>());

练习：
1 使用Collections.sort，自定义比较方式，如果年龄相同，则比较姓名，否则比较年龄
```java
@Test
void test() {
    Collections.sort(lists, (e1, e2) -> {
        if (e1.getAge() == e2.getAge()) {
            return e1.getName().compareTo(e2.getName());
        } else {
            return -Integer.compare(e1.getAge(), e2.getAge());
        }
    });
    lists.forEach(System.out::println);
}

```

2 声明函数式接口，public String getValue(String str)
  编写类，接收接口参数，字符串转大写

```java
@FunctionalInterface
public interface MyFunc {
    public String getValue(String str);
}

public strHandler(String str, MyFunc f) {
    return f.getValue(str);
}
@Test
void test(){
    String s1 = strHandler("test", (str) -> str.trim());
    String s2 = strHandler("test", (str) -> str.toUpperCase());
}
```

3 声明带有两个泛型的函数式接口，泛型类型<T, R>， T为参数，R为返回值
  声明类，计算两个long的和

```java
@FunctionalInterface
public interface MyFunc<T, R>{
    public R getValue(T t1, T t2);
}

public void op(Long l1, Long l2, MyFunc f) {
    System.out.println(f.getValue(l1, l2));
}
@Test
void test(){
    op(100L, 100L, (x, y) -> x + y);
    op(100L, 100L, (x, y) -> x * y);
}


```


### 函数式接口
接口中只有一个抽象方法的接口。
可以使用@FunctionalInterface 修饰一下

可以通过lambda表达式创建该接口的对象。
若lambada表达式抛出一个受检异常，那么该异常需要在目标接口的抽象方法上声明。

常用的接口已经内置好了

4大常用的内置接口：
1 Consumer<T> 消费型接口
  参数类型：T
  返回类型： void
  用途： 对类型为T的对象应用操作方法， void accept(T t)
2 Supplier<T>  供给型接口
  无
  T
  返回类型为T的对象， T get()
3 Function<T, R> 函数型接口
  T
  R
  返回类型为T的对象的操作结果，返回R类型  R apply(T t)
4 Predicate<T> 断定型接口
  T
  boolean
  确定T是否满足某种约束，并返回boolean   boolean test(T t)    

eg:
```java
// Consumer 消费
@Test
void test1() {
    consumer("100", (m) -> System.out.println("消费了" + m));
}
void consumer(String m, Consumer<String> con) {
    con.accept(m);
}
```

```java
// Supplier 生产
@Test
void test() {
    List<Integer> list = supplier(10, () -> (int)(Math.random()*100));
}
List<Integer> supplier(int num, Supplier<Integer> sup) {
    List<Integer> result = new ArrayList<Integer>();
    for (int i=0;i<num;i++) {
        list.add(sup.get(i));
    }
    return result;
}
```

```java
// Function<T, R> 函数
@Test
void test(){
    String s = func("123445", (s) -> s.subString(0, 2));
}
String func(String s, Function<String, String> f) {
    return f.apply(s);
}
```

```java
// Predicate<T> 断言
@Test
void test() {
    List<String> list1 = Arrays.asList("1123", "334");
    List<String> list = pred(list1, (s) -> s.length() > 3);
}
List<String> pred(List<String> list, Predicate<String> pr) {
    List<String> result = new ArrayList<String>();
    for (String s : list) {
        if (pr.test(s)) {
            result.add(s);
        }
    }
    return result;
}
```

其他接口：
BiFunction<T, U, R>  R apply(T t, U u)  (T, U) -> R
UnaryOperator<T>   T apply(T t)  (T) -> T
BinaryOperator<T>   T apply(T t1, T t2)  (T, T) -> T
BiConsumer<T, U>   void apply(T t, U u)  (T, U) -> 

ToIntFunction<T>
ToLOngFunction<T>      T -> int,long,double
ToDoubleFunction<T>  

IntFunction<R>
LOngFunction<R>       int,long,double -> R
DoubleFunction<R>  


### 方法引用与构造器引用

方法引用：
若Lambada体的内容已经有方法实现了，可以使用“方法引用”
可以理解为方法引用是lambda表达式的另外一种变现形式

有3中语法格式：

对象::实例方法名

类::静态方法名

类::实例方法名

注意：lambda体中的方法参数列表类型和返回值类型要 和 函数式接口中抽象方法的参数类型和返回值类型相同

eg:
对象::实例方法名
```java
Consumer<String> con = (x) -> System.out.println(x);

PrintStream ps = System.out;
Consumer<String> con = (x) -> ps.println(x);

Consumer<String> con = ps::println;

Consumer<String> con = System.out::println;

@Test
con.accpet("hello");


Person p = new Person();
Supplier<String> sp = () -> p.getName();

Supplier<String> sp = p::getName;

String name = sp.get();

```

eg:
类::静态方法名
```java
Comparator<Integer> con = (x, y) -> Integer.compare(x, y);
Comparator<Integer> con = Integer::compare;


```

eg:
类::实例方法名
若lambda参数列表的第一个参数是实例方法的调用者，第二个参数是实例方法的参数时，可以使用这种形式
```java
BiPredicate<String, String> bp = (x, y) -> x.equals(y);
BiPredicate<String, String> bp = String::equals;


```

构造器引用：
语法格式：
ClassName::new

注意：需要调用的构造器的参数列表 要与 函数式接口中抽象方法的参数列表保持一致

eg:
```java
Supplie<Person> sp = () -> new Person();

Supplie<Person> sp = Person::new;

Person p = sp.get();

Function<Integer, Person> fc = (age) -> new Person(age);
Function<Integer, Person> fc = Person::new;

Person p = fc.apply(100);

BiFuncton<Integer, String, Person> bf = (age, name) -> new Person(age, name);
BiFuncton<Integer, String, Person> bf = Person::new;

Person p = bf.apply(100, "lw");

```

数组引用
语法格式：
Type::new

eg:
```java
Function<Integer, String[]> fc = (length) -> new String[length];
Function<Integer, String[]> fc = String[]::new;

String[] array = fc.apply(100);
```

### Stream API
java.util.stream.*
处理集合的抽象概念

查找， 过滤 ， 映射

集合讲的是数据，流讲的是计算

注意：
Stream不会存储元素
不会改变源对象，会返回一个新的stream
stream的操作时延迟执行的。意味着等到需要结果的时候才执行。

Stream操作的3个步骤：
1 创建stream
一个数据源（集合，数组等），获取一个流
2 中间操作
一个操作中间链，对数据进行处理
3 终止操作
一个终止操作，执行中间操作链，并产生结果

Collection接口
default Stream<T> Steam() 返回一个顺序流
default Stream<E> paralleStream() 返回一个并行流

1 由数组获取流
获取数组流：
Arrays:
static <T> Stream<T> stream(T[] array)

通过重载：
public static IntStream stream(int[] array)
public static LongStream stream(long[] array)
public static DoubleStream stream(double[] array)

2 由值创建流
Stream.of ，可以接受任意参数的参数
public static<T> Stream<T> of(T... args)

3 由函数创建流：创建无限流
Stream.iterate()
Stream.generate()

//迭代
public static <T> Stream<T> iterate(final T seed, final UnaryOperator<T> f)
// 生成
public static <T> Stream<T> generate(Supplier<T> s)

eg:
// 1 创建stream
```java
// 1 通过Collection系列集合提供的stream() 或 paralleStream()
List<String> list = new ArrayList<>();
Stream<String> st1 = list.stream();

// 2 通过Arrays的静态方法stream() 获取数组流
Person[] array = new Person[100];
Stream<Person> st2 = Arrays.stream(array);

// 3 通过Stream类的静态方法 of()
Stream<String> st3 = Stream.of("a","b","c");

// 4 创建无限流
// 迭代
Stream<Integer> st4 = Stream.iterate(0, (x) -> x + 2);
// 生成
Stream<Double> st5 = Stream.generate(() -> Math.random());
st4.limit(6).forEach(System.out::println); 
```

//2 中间操作

多个中间操作可以连起来形成一个流水线，除非流水线上出发终止操作，否则中间操作不会执行任何处理。
而是在终止操作是一次性全部处理
称为“惰性操作”

**筛选与切片**
filter  接收lambda，从流中排除元素
limit   截断流，时其元素不超过给定数量
skip(n) 跳过元素， 返回一个扔掉了前n个元素的流，若不足n,则返回一个空流，与limit(n)互补
distinct  筛选， 通过流所生成的元素的hashcode和 equals 去除重复元素 

eg:
```java
List<Person> list = new ArrayList<Person>;
// 中间操作，没有终止操作不会执行, 有内部迭代来外城
Stream<Person> s = list.stream()
                        .filter((x) -> x.getAge>100)
                        .limit(2)
                        .distinct()

// 终止操作
s.forEach(System.out::println)


// 外部迭代
Interator<person> iter = list.iterator();
while(iter.hasNext()) {
    iter.next()
}
```

**映射**
map - 接收lambda，将元素转换为其他形式或者提取信息，接收一个函数作为参数，
该函数会被应用到每个元素上，并将其映射为一个新的元素。
flatMap - 接收一个函数作为参数，将流中的每一个值都换成另一个流， 然后把所有流连接成一个新流

```java
List<Person> list = Arrays.asList("1","2");
list.stream()
    .map((str) -> str.toUpperCase())
    .forEach

list1.stream()
     .map(Person::getName)
     .forEach

```


**排序**
sorted() 自然排序（Comparable）
sorted(Comparator com)  定制排序

```java
list.stream()
    .sorted()
    .forEach

list.stream()
    .sorted((x, y) -> {
        if (x.getage == y.getAge) {
            return x.getName.compareTo(y.getname)
        } else {
            return x.getage.compareTo(y.getage)
        }
    }).forEach    
```


// 3 终止操作
终止操作会从流的流水线生成结果。
结果可以不是流的任何值，如list,Integer ,void 等。

**查找与匹配**
allMatch(Predicate p)   检查是否匹配所有元素
anyMatch(Predicate p)   检查是否至少匹配一个元素
noneMatch(Predicate p)  检查是否没有匹配所有元素
findFirst()  返回第一个元素
findAny()  返回当前六中的任意元素

count()  -返回流中的总个数
max(Comparatro c)
min(Comparatro c)
forEach(Consumer c)  内部迭代

eg:
```java
public enum STATUS {
    FREE;
    BUSY;
    VOCATION;
}

boolean b1 = persons.stream()
                    .allMatch((e) -> e.getStatus.equals(Status.BUSY));

boolean b1 = persons.stream()
                    .anyMatch((e) -> e.getStatus.equals(Status.BUSY));

boolean b1 = persons.stream()
                    .noneMatch((e) -> e.getStatus.equals(Status.BUSY));


Optional<Person> op = lists.stream()
                            .sorted((e1, e2) -> Double.compare(e1.getAge, e3.getage)))
                            .findFirst();
System.out.println(op.get())                            

```


**规约**
reduce(T identity, BinaryOperator b)
reduce(BinaryOperator b)

备注：
**map 和 reduct 的连接通常称为 map-reduce模式，应为google用他来进行网络搜索**


可以将六中的元素反复结合起来，得到一个值

```java
List<Integer> list = Arrays.asList(1,3,34,55,5);
Integer m = list.stream()
                .reduct(0, (x, y) -> x + y);
Syste.out.printl(m)


Optional<Double> p = persons.stream()
                            .map(Person::getAge)
                            .reduce(Double::sum);


```

**收集**
Collect 
将流转换为其他形式。
接收一个Collector接口的实现，用于给streeam中的元素做汇总的方法

Collector接口中的方法实现决定了如何对流进行收集，如收集到set,list,map.
Collectors实用类提供了很多静态方法，可以方便的创建收集器实例。

```java
// toList
List<String> mname = prsons.stream()
                            .map(Prson::getName)
                            .collect(Collectors.toList());
// toSet
Set<String> mname = prsons.stream()
                            .map(Prson::getName)
                            .collect(Collectors.toSet());

// toCollection
Collection<Person> c = lists.stream()
                            .collect(Collectors.toCollection(ArrayList::new));


HashSet<String> mname = prsons.stream()
                            .map(Prson::getName)
                            .collect(Collectors.toCollection(HashSet::new));

// counting
Long count = presons.stream()
                    .collect(Collectors.counting());

// summingInt
int t = list.stream()
            .collect(Collectors.summingInt(Person::getAge));

// averagingDouble  averagingInt
Double db = presons.stream()
                    .collect(Collectors.averagingDouble(Person::getSarlay));

Double db = presons.stream()
                    .collect(Collectors.summingDouble(Person::getSarlay));

// max
Optional<Double> max = presons.stream()
                    .collect(Collectors.maxBy((e1, e2) -> Double.compare(e1.getsalary, e2.getsalary));

//min
Optional<Double> min = presons.stream()
                    .map(Person::getSalary)
                    .collect(Collectors.minBy(Double::compare);

// 分组
Map<Status, List<Person>> map = persons.stream()
                                    .collect(Collectors.groupingBy(Person::getStatus)); 

// 多级分组
Map<Status, Map<String, List<Person>>>  map = persons.stream()
                                    .collect(Collectors.groupingBy(Person::getStatus), Collectos.groupingBy((e) -> {
                                        if ((Person) e.getAge <= 100) {
                                            retun "老年"
                                        } else if ((Person) e.getAge <= 100) {
                                            return "中年"
                                        } else {
                                            return "少年"
                                        }
                                    })); 


// 分区
Map<Boolean, List<Person>> map = persons.stream()
                                    .collect(Collectors.partitioningBy((e) -> e.getAge > 100)); 

// summarizingDouble
DoubleSummaryStatisticss dss = persons.stream()
                                    .collect(Collectors.summarizingDouble(Prson::getSsaly);

IntSummaryStatisticss dss = persons.stream()
                                    .collect(Collectors.summarizingInt(Prson::getSsaly);
dss.getSum()
dss.getAverage()
dss.getMax()                                    

// joining
String s = persons.sream()
                    .map(Person::getName)
                    .collect(Collectos.joining(",", "====", ";"));

// reducing
int t = lists.stream()
            .collect(Collectors.reducting(0, Person::getSalary, Integer::sum));

// collectingAndThen
int t = lists.stream()
            .collect(Collectors.collectingAndThen(Collectors.toList(), List::size));

```

练习
```java
1 返回一个数列的平方数列
Integer[] n = new Integer[]{1,23,4};
Arrays.sream()
        .map((x) -> x*x)
        .forEach(System.out::println);

2 返回流的个数
Optional<Integer> op = persons.stream()
                            .map((e)->1)
                            .reduce(Integer::sum);

3 
lists.stream()
    .filter((t) -> t.getyear ==201 )
    .sorted((t1, t2) -> Intger.compare(t1.getvalue , t2.getValue))
    .forEach

lists.sream()
    .map((t) -> t.getNmae)
    .distinct()
    .forEach

lsits.stream()
    .filter((t) -> t.getname.equals("123"))
    .map(Person::getName)
    .sorted((t1,t3) -> t1.getName.compartTo(t2.getname))
    .distinct()
    .forEach    
```

并行流
数据分块，每个线程处理一个分块

parallel()  
sequential()
在并行流和串行流之间进行切换

Fork/Join 框架

任务递归拆分（fork）成小任务，
并行求值
合并（join) 求值结果


Fork/Join框架

与传统线程池的区别
采用“工作窃取”模式（work-stealing）
当执行新的任务时，可以将其拆分成更小的任务执行，并将小任务加到线程队列中，然后再从一个随机线程的队列中偷取一个放到自己的队列中

相比一般的线程池实现，fork/join框架的优势体现在对其中任务的处理上，一般的线程池，当线程处于等待时，就一直在等待状态。
而fork/join框架，如果某个子问题由于等待另外一个子问题时导致无法执行，那么线程就会主动寻找其他尚未执行的子问题来执行，
减少了线程等待时间，提高了性能。



```java
import java.util.concurrent.RecursiveTask;
public class ForkJoinCalculate extends RecursiveTask<Long> {
    private static final long serialVersionUID = 123456789L;

    private long start;
    private long end;

    private static final long THRESHOLD = 10000000L;

    public ForkJoinCalculate(long start, long end) {
        this.start = start;
        this.end = end;
    }

    @Override
    protected Long compute(){
        long length = end - start;
        if (length <= THRESHOLD) {
            long sum = 0;
            for (long i=start;i<=end;i++) {
                sum+= i;
            } 
            return sum;
            
        } else {
            long middle = (start + end) / 2;
            ForkJoinCalculate left = new ForkJoinCalculate(start, middle);
            left.fork(); // 拆分子任务，同时亚茹线程队列

            ForkJoinCaluate right = new ForkJoinCalculate(middle+1, end);
            right.fork();

            return left.join() + right.join();
        }
    }
}

import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.ForkJoinTask;

@Test
void test() {
    //long start =  System.currentTimeMillis()
    Instant start = Instant.now();

    ForkJoinPool pool = new ForkJoinPool();

    ForkJoinTask<Long> task = new ForkJoinCalculate(0, 1000000000L);
    Long sum = task.invoke(task);

    Instant end = Instant.now();

    System.out.println(Duration.between(start, end).toMilles())
}



LongStream.rangeClosed(0,100000000L)
        .parallel()
        .reduce(0, Long::sum);
```


### Optional 类
java.util.Optional 
是一个容器类，
代表一个值存在或者不存在
原来是用null表示一个值不存在，现在可以用Optional, 可以便面空指针异常


常用方法：
Optional.of(T t) 创建一个Optional实例
Optional.empty() 创建一个空的Optional实例
Optional.ofNullable(T t) 若t不为null,创建一个Optional实例，否则创建空实例
isPresent() : 判断是否包含控制
orElse(T t)  如果调用对象包含值，则返回该值，否则返回t
orElseGet(Supplier s)  如果调用对象包含值，则返回该值，否则返回s获取的值
map(Function f) 如果优质，则返回处理后的Optional，否则返回Optional.empty
flatMap(Function mapper)  与map类似，要求返回的值必须是Optional 


eg:
```java
Optional<Person> op = Optional.of(new Person());
Person p = op.get()

Optional<Person> op = Optional.ofNullable(new Person());
if (op.isPresent()) {
    Person p = op.get()
}    

Optional<Person> op = Optional.ofNullable(new Person());

Optional<String> str = op.map((e) -> e.getName)

Optional<String> str = op.flatMap((e) -> Optional.of(e.getName)));

```

### 接口中的默认方法与静态方法

java 8允许接口中包含具体的方法，称为默认方法，
默认方法用 default 修饰

eg:
```java
interface MyInterface {
    T func(int a);
    
    default String getName() {
        return "123";
    }

     // 接口中的静态方法
    public static void show() {
        Syste.ou.tprintl("1");
    }
}
```

类优先原则
若一个接口定义了默认方法，父类或者接口总又蒂尼了一个同名的方法：
选择父类的方法，如果父类提供了具体的丝线，那么默认方法被忽略
接口冲突：
如果一个父接口提供了一个默认方法，另一个接口也提供了该方法（不管是不是默认方法），那么必须覆盖该方法来解决冲突





### 新时间日期API

java 8提供了新的时间额日期的API

java.time
java.time.chrono
java.time.format
java.time.temporal
java.time.zone

LocalDate
LocalTime
LocalDateTime

LocalDate.now()
LocalTime.now()
LocalDateTime.now()

LocalDate.of(2011,1,1)
LocalTime.of(01, 02, 23)
LocalDateTime.of(2011,1,1, 01, 02, 23)

Instant

Duration
Period

OffsetTime

DateTimeFormatter


now() 
of()

plusDays
plusWeeks
plusMonths
plusYears

minusDays
minusWeeks
minusMonths


plus
minus

withDayOfMonth
withDayOfYear
withMonth
withYear

getDayOfMonth
getDayOfYear
getDayOfWeek

getMonth
getMonthvalue
getYear
util

isBefore, isAfter
isLeapYear




传统的日期类不是线程安全的
eg:
```java
public static void main(String... args) {
    SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMdd");

    Callable<Date> task = new Callable<Date>() {
        @Override
        public Date call() {
            return sdf.parse("20160101");
        }
    };

    ExecutorService pool = Executors.newFixedThreadPool(10);

    List<Future<Date>> results = new ArrayList<>();

    for (int i=0;i<10;i++) {
        results.add(pool.submit(task));
    }

    for (Future<Date> future : results) {
        System.out.println(future.get()); // 会报错，线程不安全
    }
    

}

// 用ThreadLocal来加锁

public class DateFormatLocal {
    private static final ThreadLocal<DateFormat> df = new ThreadLocal<DateFormat> (){
        protected DateFormat initialValue() {
            return new SimpleDateFormat("yyyyMMdd");
        }
    };

    public static Date convert(String resource) throws ParseException {
        return df.get().parse(source);
    }
}

public static void main(String... args) throws Exception {
    // SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMdd");

    Callable<Date> task = new Callable<Date>() {
        @Override
        public Date call() {
            return DateFormatLocal.convert("20160101");
        }
    };

    ExecutorService pool = Executors.newFixedThreadPool(10);

    List<Future<Date>> results = new ArrayList<>();

    for (int i=0;i<10;i++) {
        results.add(pool.submit(task));
    }

    for (Future<Date> future : results) {
        System.out.println(future.get()); // 会报错，线程不安全
    }   

    pool.shutdown(); 

}

// 用性的日期格式
public static void main(String... args) throws Exception {
    // SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMdd");

    DateTimeFormatter dft = DateTImeFormatter.ofPattern("yyyyMMdd");

    Callable<LocalDate> task = new Callable<LocalDate>() {
        @Override
        public LocalDate call() {
            return LocalDate.parse("20190101", dft);
        }
    };

    ExecutorService pool = Executors.newFixedThreadPool(10);

    List<Future<LocalDate>> results = new ArrayList<>();

    for (int i=0;i<10;i++) {
        results.add(pool.submit(task));
    }

    for (Future<LocalDate> future : results) {
        System.out.println(future.get()); // 会报错，线程不安全
    }   

    pool.shutdown(); 

}

```

类                                                          To 遗留类                               From 遗留类
java.time.Instant
java.util.Date                                              Date.from(instant)                     date.toInstant()

java.time.Instant   
java.sql.Timestamp                                          Timestamp.from(instant)                 timestamp.toInstant()

java.time.ZonedDateTime                               GregorianCalendar.from(zonedDateTim           cal.toZonedDateTime()
e)
java.util.GregorianCalendar
 
java.time.LocalDate
java.sql.Time                                           Date.valueOf(localDate)                         date.toLocalDate()

java.time.LocalDateTime
java.sql.Timestamp                                       Timestamp.valueOf(localDateTime)           timestamp.toLocalDateTime()

java.time.ZoneId
java.util.TimeZone                                      Timezone.getTimeZone(id)                    timeZone.toZoneId()

java.time.format.DateTimeFormatter
java.text.DateFormat                                        formatter.toFormat()                            无


老的
Date
Calendar
TimeZone  时区

以上都不是线程安全的

SimpleDateFormat 

LocalDate, LocalTIme, LocalDateTIme 类的实例是不可变的对象。
分别表示使用了ISO-8601日历系统的日期，时间， 日期和时间。
只提供了简单的日期和时间， 不包含当前的时间信息， 也不包含与时区相关的信息。

```java

@Test
void test() {
    LocalDateTime ldt = LocalDateTime.now();
    System.out.println(ldt) // 2016-01-01T11:11:11.111

    LocalDateTime ld = LocalDateTime.of(2011, 11, 12, 11, 11, 23);
    System.out.println(ldt) // 2016-01-01T11:11:11

    LocalDateTime ld = ld.pulsYear(12);

    LocalDateTime l2 = ld.minusMonths(12);

    ld.getYear();
    ld.getMonthValue()
    ld.getDayOfMonth()
    ld.getHour()
    ld.getMinute()
    lad.getSecond()

// Instant  时间戳 以unix元年： 1970。01.01 00：:00：00 到当前时间的毫秒

Instant ia = Instance.now();  // 默认获取UTC 时区，不是当前时间  2016-01-01T06:01:01.585Z

OffsetDateTime ib = ia.atOffSet(ZoneOffset.ofHours(8)); // 2016-01-01T06:01:01.585+08:00

ia.toEpochMilli()

Instant ib = Instant.ofEpochSecond(60);

// Duration 计算两个“时间”之间的间隔
// Period : 计算恋歌 “日期” 之间的间隔

Instant l1 = Instant.now();
Thread.sleep(1000);
Instant l2 = Instant.now();
Duration.between(l1,l2).toMillis()


LocalTime l1 = LocalTime.now();
Thread.sleep(1000);
LocalTime l1 = LocalTime.now();
Duration.between(l1,l2).toMillis()


LocalDate l1 = LocalDate.of(2017, 1, 1);
LocalDate l2 = LocalDate.now();
Period p = Period.between(l1,l2);
p.getYears()
p.getHours()
p.getDays()

}

```

时间校正器
TemporalAdjuster  

类TemporalAdjusters 
eg:
```java
// 获取下个周日
LocalDate nextSunday = LocalDate.now().with(
    TemporalAdjusters.next(DayOfWeek.SUNDAY)
);

LocalDateTime l = LocalDateTime.now();

LocalDateTime l1 = l.withDayOfMonth(10);

LocalDateTime l2 = l.with(TemporalAdjusters.next(DayOfWeek.SUNDAY));

LocalDateTime l3 = l.with((ll) -> {
    LocalDateTime l4 = (LocalDateTime) ll;

    DayOfWeek dow = l4.getDayOfWeek();
    if (dow.equals(DayOfWeek.FRIDAY)) {
        return l4.plusDays(3); 
    }
});

```

格式器
```java
DateTimeFormatter dtf = DateTimeFormatter.ISO_DATE;
LocalDateTime ldt = LocalDateTime.now();

String d = ldt.parse(dtf);

DateTimeFormatter dtf = DateTimeFormatter.ofPattern("yyyy年MM月dd日 HH:mm:ss");
String d = ldt.parse(dtf);

LocalDateTime n = ldt.parse(d, dft);

```

时区
ZonedDate  ZonedTime  ZonedDateTime

LocalTimeDate ldt  = LocalDateTime.now(ZoneId.of("Eurepo/Tallinn"));

LocalTimeDate ldt  = LocalDateTime.now(ZoneId.of("Asia/Shanghai"));

ZonedDateTime zdt = ldt.atZone(ZoneId.of("Asia/Shanghai"));


### 其他新特性

#### 重复注解




```java
import static java.lang.annotation.ElementType.CONSTRUCTOR;
import static java.lang.annotation.ElementType.FIELD;
import static java.lang.annotation.ElementType.LOCAL_VARIABLE;
import static java.lang.annotation.ElementType.METHOD;
import static java.lang.annotation.ElementType.PARAMETER;
import static java.lang.annotation.ElementType.TYPE;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

@Target({TYPE, FIELD, METHOD, PARAMETER, CONSTRUCTOR, LOCAL_VARIABLE})
@Retention(RetentionPolicy.RUNTIME)
public @interface myAnnotations {
    myAnnotations[] value();
}

// 可重复的注解
@Repeatable(myAnnotations.class)
@Target({TYPE, FIELD, METHOD, PARAMETER, CONSTRUCTOR, LOCAL_VARIABLE})
@Retention(RetentionPolicy.RUNTIME)
public @interface MyAnnotation {
    String value() default "123";
}

public class Test {

    @MyAnnotation("123")
    @MyAnnotation("234")
    public void test() {

    }

    @Test
    void test() {
        Class<Test> clss = Test.class;
        Method m = clss.getMethod("test");

        MyAnnotation[] mss = m.getAnnotationByType(MyAnnotation.class);

        for (MyAnnotation an : mss) {
            System.out.println(an.value());
        }
    }
}

```


#### 类型注解  ElementType.TYPE_PARAMETER


```java
import static java.lang.annotation.ElementType.TYPE_PARAMETER;

@Repeatable(myAnnotations.class)
@Target({TYPE, FIELD, METHOD, PARAMETER, CONSTRUCTOR, LOCAL_VARIABLE, TYPE_PARAMETER})
@Retention(RetentionPolicy.RUNTIME)
public @interface MyAnnotation {
    String value() default "123";
}

public class Test {

    @MyAnnotation("123")
    @MyAnnotation("234")
    public void test(@MyAnnotation("123") String name) {

    }

    @Test
    void test() {
        Class<Test> clss = Test.class;
        Method m = clss.getMethod("test");

        MyAnnotation[] mss = m.getAnnotationByType(MyAnnotation.class);

        for (MyAnnotation an : mss) {
            System.out.println(an.value());
        }
    }
}

```

