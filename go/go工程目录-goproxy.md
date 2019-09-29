Go工程目录说明

Go语言的项目结构有做规定，做到了统一。


一个Go项目在GOPATH下，一般会有3个目录
- src 存放项目源文件(.go)
- bin 存放编译后的可执行文件(.exe)
- pkg 存放编译后的包文件,一般文件后缀(.a)，pkg中的文件是Go编译生成的

一般，bin和pkg目录可以不创建，go命令会自动创建(如 go install),只需要创建src目录即可。
对于src目录，存放源文件，Go中源文件以包（package）的形式组织。通常，**新建一个包就在src目录中新建一个文件夹。**

```
-- 1
firstGo--
    |--src

-- 2
为了编译方便，增加了一个install文件
加上这个install.bat，是不用配置GOPATH（避免新增一个GO项目就要往GOPATH中增加一个路径）
firstGo--
    |--src
    |--install.bat

-- 3
接下来，增加一个包：config和一个main程序
myfirst
    |—instal.bat
    |—src
        |-- config    
            |-- config.go
        |—myfirst        
            |-- main.go

注意:
config.go中的package名称必须最好和目录config一致，而文件名可以随便。
main.go表示main包，文件名建议为main.go。
（注：不一致时，生成的.a文件名和目录名一致，这样，在import 时，应该是目录名，而引用包时，需要包名。
例如：目录为myconfig，包名为config，则生产的静态包文件是：myconfig.a，引用该包：import “myconfig”，
使用包中成员：config.LoadConfig()）


```



```shell
go env -w GOPROXY=https://goproxy.cn,direct

```