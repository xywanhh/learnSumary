从大学二年级接触java，有几年了，今天才知道从JavaSE2以后，就划分了几种引用。之前一直知道的是参数有值传递和引用传递。原来引用也分几种类型。

4种引用类型：
1 强引用
2 软引用
3 弱引用
4 虚引用

虽然Jvm帮助我们管理了对象的回收，对象在内存中都是放在堆里。对象基本都是引用类型，但是针对不同引用类型，jvm是区别对待的。
只要对象有强引用，jvm必定不回收这个对象，即使内存不足，宁愿抛出OutOfMemory错误也不回收。
对于软引用关联的对象，只有在内存不足的时候，jvm才会回收。
当jvm进行垃圾回收时，无论内存是否充足，都会回收被弱引用关联的对象。
如果一个对象与虚引用进行关联，则跟没有跟引用关联一样，任何时候都可能被垃圾回收。

根据垃圾回收对于不同引用类型的区别对待来说，善于利用软引用和弱引用，可以有效避免OOM错误。

强引用(StrongReference)：
平时编码中普遍存在。
```java
public class Main {
    public static void main(String... args) {
        new Main().func1();
        // 函数执行完之后，对象才有可能被gc
    }

    public void func1() {
        // 以下对象都是强引用
        Object obj1 = new Object();
        // 只要某个对象有强引用与之关联，JVM必定不会回收这个对象，即使在内存不足的情况下，JVM宁愿抛出OutOfMemory错误也不会回收这种对象。
        // 当运行至Object[] objArr = new Object[1000];这句时，如果内存不足，JVM会抛出OOM错误也不会回收object指向的对象。
        Object[] objArr = new Object[1000];
    }
}
```

问题来了，如果非要在强引用的时候，来回收这个对象，怎么办？
jdk中，Vector的remove(int) 方法提供了一个方案。
```java
public synchronized E remove(int index) {
    ++this.modCount;
    if (index >= this.elementCount) {
        throw new ArrayIndexOutOfBoundsException(index);
    } else {
        E oldValue = this.elementData(index);
        int numMoved = this.elementCount - index - 1;
        if (numMoved > 0) {
            System.arraycopy(this.elementData, index + 1, this.elementData, index, numMoved);
        }
        // 如果想中断强引用和某个对象之间的关联，可以显示地将引用赋值为null，这样一来的话，JVM在合适的时间就会回收该对象
        this.elementData[--this.elementCount] = null;
        return oldValue;
    }
}
```

软引用(SoftReference)：
软引用用来描述一些有用但不是必须的对象。
java.lang.ref.SoftReference
对于软引用关联着的对象，只有在内存不足的时候JVM才会回收该对象。
这一点可以很好地用来解决OOM的问题，并且这个特性很适合用来实现缓存：比如网页缓存、图片缓存等。

软引用可以和一个引用队列（ReferenceQueue）联合使用，如果软引用所引用的对象被JVM回收，这个软引用就会被加入到与之关联的引用队列中。
```java
import java.lang.ref.SoftReference;

public class Main {
    SoftReference<String> str = new SoftReference<String>(new String("hello"));
    System.out.println(str.get());
}
```

弱引用(WeakReference):
弱引用也是用来描述一些有用但不是必须的对象。
java.lang.ref.WeakReference
当JVM进行垃圾回收时，无论内存是否充足，都会回收被弱引用关联的对象。

不过要注意的是，这里所说的被弱引用关联的对象是指只有弱引用与之关联，如果存在强引用同时与之关联，则进行垃圾回收时也不会回收该对象（软引用也是如此）。

弱引用可以和一个引用队列（ReferenceQueue）联合使用，如果弱引用所引用的对象被JVM回收，这个软引用就会被加入到与之关联的引用队列中。

```java
import java.lang.ref.WeakReference;

public class Main {
    WeakReference<String> str = new WeakReference<String>(new String("hello"));
    System.gc(); // 通知gc进行垃圾回收
    System.out.println(str.get());
}
// hello
// null
```

虚引用(PhantomReference):
前面的软引用、弱引用不同，它并不影响对象的生命周期。
java.lang.ref.PhantomReference
如果一个对象与虚引用关联，则跟没有引用与之关联一样，在任何时候都可能被垃圾回收器回收。
要注意的是，虚引用必须和引用队列关联使用。

当垃圾回收器准备回收一个对象时，如果发现它还有虚引用，就会把这个虚引用加入到与之 关联的引用队列中。
程序可以通过判断引用队列中是否已经加入了虚引用，来了解被引用的对象是否将要被垃圾回收。
如果程序发现某个虚引用已经被加入到引用队列，那么就可以在所引用的对象的内存被回收之前采取必要的行动。

```java
import java.lang.ref.PhantomReference;

public class Main {
    ReferenceQueue<String> queue = new ReferenceQueue<String>();
    PhantomReference<String> pr = new PhantomReference<String>(new String("hello"), queue);
    System.out.println(pr.get());

}
// hello
// null
```

在实际应用中，可能有个场景，需要在一个数据结构中存储过多的数据，这个时候，可能会在服务器内存不足的情况下，报OOM错误。
这个时候，如果存储这个数据的结构都和软引用进行关联，那么在内存不足的时候，jvm一定会进行gc，从而避免了OOM。

下面这个例子，下载大量图片的场景：
假如有一个应用需要读取大量的本地图片，如果每次读取图片都从硬盘读取，则会严重影响性能，但是如果全部加载到内存当中，又有可能造成内存溢出，此时使用软引用可以解决这个问题。

设计思路是：用一个HashMap来保存图片的路径 和 相应图片对象关联的软引用之间的映射关系，在内存不足时，JVM会自动回收这些缓存图片对象所占用的空间，从而有效地避免了OOM的问题。在Android开发中对于大量图片下载会经常用到。

```java
private Map<String, SoftReference<Bitmap>> imageCache = new Hash<String, SoftReference<Bitmap>>();

public void addBitmapToCache(String path) {
    // 强引用的Bitmap对象
    Bitmap bitmap = BitmapFactory.decodeFile(path);

    // 软引用的Bitmap对象
    SoftReference<Bitmap> softBitmap = new SoftReference<Bitmap>(bitmap);

    // 添加该对象到Map中使其缓存
    imageCache.put(path, softBitmap);
}

public void Bitmap getBitmapByPath(String path) {
    // 从缓存中取软引用的Bitmap对象
    SoftReference<Bitmap> softBitmap = imageCache.get(path);
    
    // 判断是否存在软引用
    if (softBitmap == null) {
        return null;
    }

    // 还存在软引用，说明对象还没被回收
    Bitmap bitmap = softBitmap.get();
    return bitmap;
}
```

这里贴下源码：
```java
package java.lang.ref;

public class SoftReference<T> extends Reference<T> {
    private static long clock;
    private long timestamp;

    public SoftReference(T referent) {
        super(referent);
        this.timestamp = clock;
    }

    public SoftReference(T referent, ReferenceQueue<? super T> q) {
        super(referent, q);
        this.timestamp = clock;
    }

    public T get() {
        T o = super.get();
        if (o != null && this.timestamp != clock) {
            this.timestamp = clock;
        }

        return o;
    }
}

```