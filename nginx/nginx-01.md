
## 1 概念
> 是高性能的http和反向代理的服务器。

### 1.1 反向代理
> 正向代理

### 1.2 负载均衡

### 1.3 动静分离
为了加快网站的解析速度，可以把动态页面和静态页面由不同的服务器来解析，加快解析速度，降低原来单个服务器的压力。


## 2 实战

### 2.1 反向代理实例

### 2.2 负载均衡实例

### 2.3 动静分离实例


### 2.4 高可用实例（高并发，分布式）

## 3 安装，常用命令和配置文件
Nginx 是 C语言 开发，建议在 Linux 上运行，当然，也可以安装 Windows 版本。

一. gcc 安装
安装 nginx 需要先将官网下载的源码进行编译，编译依赖 gcc 环境，如果没有 gcc 环境，则需要安装：
yum -y install gcc automake autoconf libtool make
yum install gcc gcc-c++

二、选定安装文件目录
cd /usr/local/src

三、安装PCRE库
PCRE(Perl Compatible Regular Expressions) 是一个Perl库，包括 perl 兼容的正则表达式库。nginx 的 http 模块使用 pcre 来解析正则表达式，所以需要在 linux 上安装 pcre 库，pcre-devel 是使用 pcre 开发的一个二次开发库。nginx也需要此库。命令：
yum install -y pcre pcre-devel


/# 下载安装包
wget ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/pcre-8.39.tar.gz 
/# 解压文件
tar -zxvf pcre-8.37.tar.gz
cd pcre-8.34
./configure
make
make install

四、安装zlib库
zlib 库提供了很多种压缩和解压缩的方式， nginx 使用 zlib 对 http 包的内容进行 gzip ，所以需要在 Centos 上安装 zlib 库。
yum install -y zlib zlib-devel

/# 进入目录中
cd /usr/local/src
 
/# 下载安装文件
wget http://zlib.net/zlib-1.2.11.tar.gz
/# 解压文件
tar -zxvf zlib-1.2.11.tar.gz
cd zlib-1.2.11
./configure
make
make install

五、安装openssl（某些vps默认没装ssl)
OpenSSL 是一个强大的安全套接字层密码库，囊括主要的密码算法、常用的密钥和证书封装管理功能及 SSL 协议，并提供丰富的应用程序供测试或其它目的使用。
nginx 不仅支持 http 协议，还支持 https（即在ssl协议上传输http），所以需要在 Centos 安装 OpenSSL 库。
若wget方法不可使用，也可以把安装包下载后上传到服务器上
cd /usr/local/src
wget https://www.openssl.org/source/openssl-1.0.1t.tar.gz
tar -zxvf openssl-1.0.1t.tar.gz

yum install -y openssl openssl-devel

六、安装nginx
cd /usr/local/src
 
wget http://nginx.org/download/nginx-1.1.10.tar.gz
tar -zxvf nginx-1.1.10.tar.gz
cd nginx-1.1.10
./configure
make
make install

确保系统已经安装了wget，如果没有安装，执行 yum install wget 安装。
wget -c https://nginx.org/download/nginx-1.12.0.tar.gz

或者
官网下载
1.直接下载.tar.gz安装包，地址：https://nginx.org/en/download.html

解压
依然是直接命令：

tar -zxvf nginx-1.12.0.tar.gz
cd nginx-1.12.0

配置
其实在 nginx-1.12.0 版本中你就不需要去配置相关东西，默认就可以了。当然，如果你要自己配置目录也是可以的。
1.使用默认配置

./configure
2.自定义配置（不推荐）

./configure \
--prefix=/usr/local/nginx \
--conf-path=/usr/local/nginx/conf/nginx.conf \
--pid-path=/usr/local/nginx/conf/nginx.pid \
--lock-path=/var/lock/nginx.lock \
--error-log-path=/var/log/nginx/error.log \
--http-log-path=/var/log/nginx/access.log \
--with-http_gzip_static_module \
--http-client-body-temp-path=/var/temp/nginx/client \
--http-proxy-temp-path=/var/temp/nginx/proxy \
--http-fastcgi-temp-path=/var/temp/nginx/fastcgi \
--http-uwsgi-temp-path=/var/temp/nginx/uwsgi \
--http-scgi-temp-path=/var/temp/nginx/scgi

注：将临时文件目录指定为/var/temp/nginx，需要在/var下创建temp及nginx目录

编译安装
make
make install

make & make install

查找安装路径：

whereis nginx
/usr/local/nginx/sbin/

启动、停止nginx
cd /usr/local/nginx/sbin/
./nginx 
./nginx -s stop
./nginx -s quit
./nginx -s reload

验证nginx配置文件是否正确
进入nginx安装目录sbin下，输入命令

./nginx -t
看到如下显示nginx.conf syntax is ok

nginx.conf test is successful

说明配置文件正确！

/nginx/conf/nginx.conf

linux默认防火墙是不开放80端口的
firewall-cmd --list-all
设置开放的端口
firewall-cmd -add-service=http -permanent
sudo firewall-cmd --add-port=80/tcp --permanent
重启防火墙
firewall-cmd --reload


CentOS6.8防火墙关闭命令
注：以下是防火墙的基本操作命令，可能但不一定适用于所有centos系统：

查询防火墙状态:

[root@localhost ~]# service iptables status

停止防火墙:

[root@localhost ~]# service iptables stop

启动防火墙:

[root@localhost ~]# service iptables start

重启防火墙:

[root@localhost ~]# service iptables restart

永久关闭防火墙:

[root@localhost ~]# chkconfig iptables off

永久关闭后启用:

[root@localhost ~]# chkconfig iptables on

常用命令：


## 4 原理


