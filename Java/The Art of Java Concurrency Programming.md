
从 JDK源码,JVM， CPU 角度
讲解
并发框架的原理和核心技术

http://ifeve.com

并发的目的： 为了让程序跑的更快

但并不是线程越多，就越快。
线程带来的：
- 上下文切换问题
- 死锁问题
- 受限于硬件和软件

上下文切换：
> CPU给每个线程分配CPU时间片
> CPU时间片是各个线程的执行时间，但是非常短，一般是几十毫秒
> CPU不停切换时间片，所以也在不停切换线程
> 时间片分配算法
> 在切换之前会保存任务的状态，切换回来时会再加载这个任务的状态
> 任务从保存到再加载的过程称为一次上下文切换

eg:
演示多线程不一定比单线程快
```java
public class Test {
    private static final long count = 100000l; // 在百万以下，多线程要更耗时，线程有创建和上下文切换的开销
    public static void main(String... args) {
        concurrency();
        serial();
    }

    private static void concurrency() throws InterruptedException {
        long start = System.currentTimeMillis();
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                int a = 0;
                for (long i = 0; i < count; i++) {
                    a += 5;
                }
            }
        });
        thread.start();
        
        int b = 0;
        for (long i = 0; i < count; i++) {
            b--;
        }

        long start = System.currentTimeMillis() - start;
        thread.join();
        System.out.println("concurrency: " + time + "ms, b = " + b);

    }

    private static void serial() {
        long start = System.currentTimeMillis();
        
        int a = 0;
        for (long i = 0; i < count; i++) {
            a += 5;
        }
        
        int b = 0;
        for (long i = 0; i < count; i++) {
            b--;
        }

        long start = System.currentTimeMillis() - start;
        thread.join();
        System.out.println("serial: " + time + "ms, b = " + b);

    }
    
}

```

性能测试工具：
Lmbench3

vmstat
- CS(Content Switch)

较少上下文切换：
- 无锁并发编程，多线程竞争锁时，会引发上下文切换，可以使用一些办法避免使用锁，比如讲数据ID按照hash算法取模分段，不同线程处理不同段
- CAS算法，比如Atomic包，不需要锁
- 使用最少线程和使用协程

jstack命令dump线程信息，看下pid 3117的进程里的线程在干啥
$ sudo -u admin /opt/ifeve/java/bin/jstack 31117 > /home/tengfei.fangtf/dump17
统计线程都处于什么状态：
$ grep java.lang.Thread.State dump17 | awk '{print $2$3$4$5}' | sort | uniq -C

线程每一次从waiting 到 runnable都会进行一次上下文切换。

死锁演示：
```java
public class DeadLock {
    private static String A = "A";
    private static String B = "B";

    public static void main(String... args) {
        new DeadLock.deadLock();
    }

    private void deadLock() {
        Thread t1 = new Thread(new Runnable() {
            @Override
            public void run() {
                synchronized (A) {
                    try {
                        Thread.sleep(10000);
                    } catch(InterruptedException e) {
                        e.printStackTrace();
                    }
                    synchronized (B) {
                        System.out.println(B);
                    }
                }
            }
        });

        Thread t2 = new Thread(new Runnable() {
            @Override
            public void run() {
                synchronized (B) {
                    synchronized (A) {
                        System.out.println(A);
                    }
                }
            }
        });

        t1.start();
        t2.start();
    }
}
```

避免死锁：
- 避免一个线程同时获得多个锁
- 避免一个线程在锁内占用多个资源，尽量保证每个锁只占用一个资源
- 尝试使用定时锁，使用lock.tryLock(timeout)来代替使用内部锁机制
- 对于数据库锁，加锁和解锁必须在一个数据库连接里，否则会出现解锁失败


资源限制：
- 网络的带宽
- 硬盘的读写速度
- cpu的处理速度
- 数据库连接数
- Socket连接数

解决：
- 集群，数据ID % 机器数，计算一个机器编号
- 资源池复用，数据库连接池，Socket连接复用

建议多使用JDK并发包提供的并发容器和工具类。

### 并发机制的底层实现：

Java源码 
--> 编译
Java字节码
-->
类加载器
-->
JVM
-->
汇编指令
-->
CPU

并发机制依赖于JVM的实现和CPU的指令

volatile ：
> 运行线程访问共享变量，为了确保变量能够被准确和一致的更新，线程应该确保通过排他锁单独获得这个变量。
> 如果一个字段被声明为volatile，java线程内存模型确保所有线程看到这个变量的值是一致的。


轻量级的synchronized，
在多处理器开发中保证了共享变量的可见性
可见性的意思是当一个线程修改了一个共享变量时，另外一个变量能够读到这个修改的值，
不会引起上下文切换



Java中每个对象都可以当做锁。
- 普通的同步方法，锁时当前实例对象
- 静态同步方法，锁时当前类的Class对象
- 同步方法块，锁是Synchronized括号里的对象

synchronized用的锁
- 存放在Java对象头里。

Java对象头里都有：
对象的hashCode, 锁信息， 对象类型的指针， 数组的长度，分代年龄， 锁标记位


级别从低到高：
- 无锁
- 偏向锁
- 轻量级锁
- 重量级锁

锁可以升级不能降级，目的是为了提高获得锁和释放锁的效率。


#### 偏向锁:
> 使用了一种等到锁竞争出现才释放锁的机制，
所以当其他线程竞争偏向锁是，偏向锁才会释放锁

![偏向锁初始化流程](./images/pianxiangsuo1.png)

默认开启的，一般启动后几秒后激活
可以使用JVM参数来关闭延迟：
-XX:BiasedLockingStartupDelay=0
关闭偏向锁：
-XX:-UseBiasedLocking=false

关闭后，默认进入轻量级锁

#### 轻量级锁：
> 线程在执行同步块之前，
> JVM会先在当前线程的栈桢中创建用于存储锁记录的空间，
> 并将对象头中的Mark Word复制到锁记录中，官方称为Displaced Mark Word。
> 然后线程尝试使用CAS将对象头中的Mark Word替换为指向锁记录的指针。
> 如果成功，当前线程获得锁，
> 如果失败，表示其他线程竞争锁，当前线程便尝试使用自旋来获取锁


轻量级锁解锁:
> 轻量级解锁时，会使用原子的CAS操作将Displaced Mark Word替换回到对象头，
> 如果成功，则表示没有竞争发生。
> 如果失败，表示当前锁存在竞争，锁就会膨胀成重量级锁

![争夺锁导致的锁膨胀流程](./images/suopengzhang1.png)


轻量级锁的缺陷：
> 因为自旋会消耗CPU，为了避免无用的自旋（比如获得锁的线程被阻塞住了），一旦锁升级
成重量级锁，就不会再恢复到轻量级锁状态。当锁处于这个状态下，其他线程试图获取锁时，
都会被阻塞住，当持有锁的线程释放锁之后会唤醒这些线程，被唤醒的线程就会进行新一轮
的夺锁之争

锁的对比：
![对比](./images/suo1.png)


在Java中，实现原子操作：
- 通过锁
- 循环CAS的方式来


#### 使用循环CAS实现原子操作
> JVM中的CAS操作正是利用了处理器提供的CMPXCHG指令实现的。
> 自旋CAS实现的基本思路就是循环进行CAS操作直到成功为止，

eg:
以下代码实现了一个基于CAS线程安全的计数器
方法safeCount和一个非线程安全的计数器count
```java
private AtomicInteger atomicI = new AtomicInteger(0);
private int i = 0;

public static void main(String... args) {
    final Counter cas = new Counter();
    List<Thread> ts = new ArrayList<Thread>(600);
    long start = System.currentTimeMillis();
    for (int i = 0; i < 100; i++) {
        Thread t = new Thread(new Runnable() {
            @Override
            public void run() {
                for (int j = 0; j < 10000; j++) {
                    cas.count();
                    cas.safeCount();
                }
            }
        });

        ts.add(t);
    }

    for (Thread t: ts) {
        t.start();
    }

    // 等待所有线程执行完成
    for (Thread t : ts) {
        try {
            t.join();
        } catch(InterruptedException e) {
            e.printStackTrace();
        }
    }

    System.out.println(cas.i);
    System.out.println(cas.atomicI.get());
    System.out.println(System.currentTimeMillis() - start);
}

// 使用CAS实现的线程安全的计数器
private void safeCount() {
    for (;;) {
        int i = atomicI.get();
        boolean suc = atomicI.compareAndSet(i, ++i);
        if (suc) {
            break;
        }
    }
}

// 非线程安全
private void count() {
    i++;
}

```

JDK提供了类来进行原子操作：
AtomicBoolean
AtomicInteger
AtomicLong
AtomicReference


CAS实现原子操作有3大问题：
- ABA 问题 （解决思路是在变量前加版本号，AtomicStampedReference）
- 循环时间长开销大
- 只能保证一个共享变量的原子操作(AtomicReference，放在一个对象里)

```java
public boolean compareAndSet(
    V expectedReference, // 预期引用
    V newReference, // 更新后的引用
    int expectedStamp, // 预期标志
    int newStamp // 更新后的标志
)
```


#### 使用锁机制实现原子操作
> 锁机制保证了只有获得锁的线程才能够操作锁定的内存区域。
> JVM内部实现了很多种锁机制，有偏向锁、轻量级锁和互斥锁。
> 除了偏向锁，JVM实现锁的方式都用了循环CAS，即当一个线程想进入同步块的时候使用循环CAS的方式来获取锁，当它退出同步块的时候使用循环CAS释放锁。


Java中的大部分容器和框架都依赖于volatile和原子操作的实现原理


### Java内存模型

Java线程之间的通信对程序员完全透明，内存可见性问题很容易困扰Java程序员。

重排序
顺序一致性内存模型

在并发编程中，需要处理两个关键问题：
- 线程之间如何通信
- 及线程之间如何同步

通信是指线程之间以何种机制来交换信息

线程之间的通信机制有两种：
- 共享内存
- 消息传递

> 在共享内存的并发模型里，线程之间共享程序的公共状态，通过写-读内存中的公共状态
进行隐式通信。

> 在消息传递的并发模型里，线程之间没有公共状态，线程之间必须通过发送消
息来显式进行通信


> Java的并发采用的是共享内存模型，
> Java线程之间的通信总是隐式进行，整个通信过程对程序员完全透明。
> 如果编写多线程程序的Java程序员不理解隐式进行的线程之间通信的工作机制，很可能会遇到各种奇怪的内存可见性问题

Java内存模型的抽象结构：
> 在Java中，所有实例域、静态域和数组元素都存储在堆内存中，
> 堆内存在线程之间共享（本章用“共享变量”这个术语代指实例域，静态域和数组元素）。
> 局部变量（Local Variables），方法定义参数（Java语言规范称之为Formal Method Parameters）和异常处理器参数（ExceptionHandler Parameters）不会在线程之间共享，它们不会有内存可见性问题，也不受内存模型的影响。


从抽象的角度来看，JMM定义了线程和主内存之间的抽
象关系：
> 线程之间的共享变量存储在主内存（Main Memory）中，
> 每个线程都有一个私有的本地内存（Local Memory），本地内存中存储了该线程以读/写共享变量的副本。
> 本地内存是JMM的一个抽象概念，并不真实存在。它涵盖了缓存、写缓冲区、寄存器以及其他的硬件和编译器优化

![Java内存模型抽象](./images/neicunmx1.png)


如果线程A与线程B之间要通信的话，必须要经历下面2个步骤。
- 1）线程A把本地内存A中更新过的共享变量刷新到主内存中去。
- 2）线程B到主内存中去读取线程A之前已更新过的共享变量。

![Java内存模型抽象](./images/neicun2.png)


在执行程序时，为了提高性能，编译器和处理器常常会对指令做重排序。

重排序分3种类型：
- 1）编译器优化的重排序。编译器在不改变单线程程序语义的前提下，可以重新安排语句
的执行顺序。
- 2）指令级并行的重排序。现代处理器采用了指令级并行技术（Instruction-Level
Parallelism，ILP）来将多条指令重叠执行。如果不存在数据依赖性，处理器可以改变语句对应
机器指令的执行顺序。
- 3）内存系统的重排序。由于处理器使用缓存和读/写缓冲区，这使得加载和存储操作看上
去可能是在乱序执行

总结：
从Java源代码到最终实际执行的指令序列，会分别经历下面3种重排序：

源代码  --》 1 编译器 优化重排序 ---》 2 指令级 并行重排序  ---》 3 内存系统 重排序 ---》 最终执行的指令序列

从JDK 5开始，Java使用新的JSR-133内存模型
JSR-133使用happens-before的概念来阐述操作之间的内存可见性

在JMM中，如果一个操作执行的结果需要对另一个操作可见，那么这两个操作之间必须要存在happens-before关
系。
这里提到的两个操作既可以是在一个线程之内，也可以是在不同线程之间


#### happens-before规则如下。
- ·程序顺序规则：一个线程中的每个操作，happens-before于该线程中的任意后续操作。
- ·监视器锁规则：对一个锁的解锁，happens-before于随后对这个锁的加锁。
- ·volatile变量规则：对一个volatile域的写，happens-before于任意后续对这个volatile域的读。
- ·传递性：如果A happens-before B，且B happens-before C，那么A happens-before C。


#### 重排序
> 是指编译器和处理器为了优化程序性能而对指令序列进行重新排序的一种手段

#### as-if-serial
> 语义的意思是：不管怎么重排序（编译器和处理器为了提高并行度），（单线程）程序的执行结果不能被改变。编译器、runtime和处理器都必须遵守as-if-serial语义


#### 顺序一致性内存模型
> 是一个理论参考模型，在设计的时候，处理器的内存模型和编程语言的内存模型都会以顺序一致性内存模型作为参照


当声明共享变量为volatile后，对这个变量的读/写将会很特别


#### volatile的内存语义

理解volatile特性的一个好方法是把对volatile变量的单个读/写，看成是使用同一个锁对这些单个读/写操作做了同步

eg:
```java
class VolatileFeaturesExample {
    volatile long vl = 0L; // 使用volatile声明64位的long型变量
    public void set(long l) {
    vl = l; // 单个volatile变量的写
    }
    public void getAndIncrement () {
    vl++; // 复合（多个）volatile变量的读/写
    } 
    public long get() {
        return vl; // 单个volatile变量的读
    }
}

// 这个程序在语义上和下面程序等价
class VolatileFeaturesExample {
    long vl = 0L; // 64位的long型普通变量
    public synchronized void set(long l) { // 对单个的普通变量的写用同一个锁同步
    vl = l;
    }
    public void getAndIncrement () { // 普通方法调用
        long temp = get(); // 调用已同步的读方法
        temp += 1L; // 普通写操作
        set(temp); // 调用已同步的写方法
    }
    public synchronized long get() { // 对单个的普通变量的读用同一个锁同步
        return vl;
    }
}

```


简而言之，volatile变量自身具有下列特性：
- ·可见性。对一个volatile变量的读，总是能看到（任意线程）对这个volatile变量最后的写入
- ·原子性：对任意单个volatile变量的读/写具有原子性，但类似于volatile++这种复合操作不具有原子性

从JSR-133开始（即从JDK5开始），volatile变量的写-读可以实现线程之间的通信

> 从内存语义的角度来说，volatile的写-读与锁的释放-获取有相同的内存效果：
> volatile写和锁的释放有相同的内存语义；
> volatile读与锁的获取有相同的内存语义


volatile写的内存语义如下: 
> 当写一个volatile变量时，JMM会把该线程对应的本地内存中的共享变量值刷新到主内存

volatile读的内存语义如下: 
> 当读一个volatile变量时，JMM会把该线程对应的本地内存置为无效。线程接下来将从主内存中读取共享变量


由于volatile仅仅保证对单个volatile变量的读/写具有原子性，
而锁的互斥执行的特性可以确保对整个临界区代码的执行具有原子性。
在功能上，锁比volatile更强大；
在可伸缩性和执行性能上，volatile更有优势。
如果想在程序中用volatile代替锁，请一定谨慎


#### 锁的内存语义

锁是Java并发编程中最重要的同步机制。
锁除了让临界区互斥执行外，
还可以让释放锁的线程向获取同一个锁的线程发送消息

> 当线程释放锁时，JMM会把该线程对应的本地内存中的共享变量刷新到主内存中
>> 当线程获取锁时，JMM会把该线程对应的本地内存置为无效。从而使得被监视器保护的
临界区代码必须从主内存中读取共享变量


- 锁释放与volatile写有相同的内存语义；
- 锁获取与volatile读有相同的内存语义


#### 锁释放和锁获取的内存语义: 
- ·线程A释放一个锁，实质上是线程A向接下来将要获取这个锁的某个线程发出了（线程A对共享变量所做修改的）消息。
- ·线程B获取一个锁，实质上是线程B接收了之前某个线程发出的（在释放这个锁之前对共享变量所做修改的）消息。
- ·线程A释放锁，随后线程B获取这个锁，这个过程实质上是线程A通过主内存向线程B发送消息


#### 锁内存语义的实现

