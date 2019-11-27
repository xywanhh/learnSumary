package com.ntools.hw.springBootDemo.test.strings;

/**
 * 测试String类会表现出值类型的特性吗？
 */
public class MainActivity {
    public static void main(String[] args) {
        System.out.println(test1());
        System.out.println(test2());
        System.out.println(test3());
    }

    static boolean test1() {
        String a = "123";
        String b = "123";
        return a == b; // true
    }

    static boolean test2() {
        String a = new String("123");
        String b = new String("123");
        return a == b; // false
    }

    static boolean test3() {
        String a = "123";
        String b = new String("123");
        return a == b; // false
    }
}
