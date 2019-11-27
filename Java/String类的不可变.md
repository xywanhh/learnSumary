String类的一个最大特性是不可修改性。

原因是在String内部定义了一个常量数组，因此每次对字符串的操作实际上都会另外分配分配一个新的常量数组空间（这片空间位于jvm的静态方法区）

查看源码：
```java
// final修饰，不可修改，不可继承
// 实现了Serializable，Comparable接口
// 所有的成员方法都默认为final方法。
public final class String implements Serializable, Comparable<String>, CharSequence

@Stable
private final byte[] value; // 内部维护了一个final常量数组，用来存放值

// 有多个重载的构造方法
public String() {
    this.value = "".value;
    this.coder = "".coder;
}

public int length() {
    return this.value.length >> this.coder();
}

public boolean isEmpty() {
    return this.value.length == 0;
}

public char charAt(int index) {
    return this.isLatin1() ? StringLatin1.charAt(this.value, index) : StringUTF16.charAt(this.value, index);
}

public int indexOf(int ch) {
    return this.indexOf(ch, 0);
}
public int indexOf(int ch, int fromIndex) {
    return this.isLatin1() ? StringLatin1.indexOf(this.value, ch, fromIndex) : StringUTF16.indexOf(this.value, ch, fromIndex);
}

// System.arraycopy(Object src, int srcPos, Object dest, int destPos, int length)是在底层操作中经常用到的一个数组复制方法
void getChars(char dst[], int dstBegin) {
    System.arraycopy(value, 0, dst, dstBegin, value.length);
}

// 重写了equals方法
public boolean equals(Object anObject) {
    if (this == anObject) {
        return true;
    } else {
        if (anObject instanceof String) {
            String aString = (String)anObject;
            if (this.coder() == aString.coder()) {
                return this.isLatin1() ? StringLatin1.equals(this.value, aString.value) : StringUTF16.equals(this.value, aString.value);
            }
        }
        return false;
    }
}
// 重写了hashcode方法
public int hashCode() {
    int h = this.hash;
    if (h == 0 && this.value.length > 0) {
        this.hash = h = this.isLatin1() ? StringLatin1.hashCode(this.value) : StringUTF16.hashCode(this.value);
    }

    return h;
}

// 截取，返回新串
public String substring(int beginIndex) {
    if (beginIndex < 0) {
        throw new StringIndexOutOfBoundsException(beginIndex);
    } else {
        int subLen = this.length() - beginIndex;
        if (subLen < 0) {
            throw new StringIndexOutOfBoundsException(subLen);
        } else if (beginIndex == 0) {
            return this;
        } else {
            // 注意这里是以长度取子字符串，因此endIndex最后一位不会被取，返回新的字符串
            return this.isLatin1() ? StringLatin1.newString(this.value, beginIndex, subLen) : StringUTF16.newString(this.value, beginIndex, subLen);
        }
    }
}

// 连接两个字符串，返回新串
public String concat(String str) {
    int olen = str.length();
    if (olen == 0) {
        return this;
    } else {
        byte[] buf;
        if (this.coder() == str.coder()) {
            byte[] val = this.value;
            buf = str.value;
            int len = val.length + buf.length;
            byte[] buf = Arrays.copyOf(val, len);
            // 现将原先的字符串复制到缓存数组中
            System.arraycopy(buf, 0, buf, val.length, buf.length);
            return new String(buf, this.coder);
        } else {
            int len = this.length();
            buf = StringUTF16.newBytesFor(len + olen);
            this.getBytes(buf, 0, (byte)1);
            str.getBytes(buf, len, (byte)1);
            return new String(buf, (byte)1);
        }
    }
}

// 将字符串储存到字节数组中
public char[] toCharArray() {
    return this.isLatin1() ? StringLatin1.toChars(this.value) : StringUTF16.toChars(this.value);
}

// native
public native String intern();

```

StringBuffer、StringBuilder和String一样，也用来代表字符串。

String类是不可变类，任何对String的改变都会引发新的String对象的生成；
StringBuilder/StringBuffer则是可变类，任何对它所指代的字符串的改变都不会产生新的对象，
而StringBuffer则是线程安全版的StringBuilder。

源码：
```java
// 线程安全
public final class StringBuffer extends AbstractStringBuilder implements Serializable, Comparable<StringBuffer>, CharSequence 
private transient String toStringCache;
public synchronized StringBuffer append(Object obj) {
    this.toStringCache = null;
    super.append(String.valueOf(obj));
    return this;
}

// 非线程安全
public final class StringBuilder extends AbstractStringBuilder implements Serializable, Comparable<StringBuilder>, CharSequence 
public StringBuilder append(Object obj) {
    return this.append(String.valueOf(obj));
}

```