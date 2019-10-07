# ZCELIB

**ZCELIB**（ZEN Communication Environment Libary）是我课余开发的一个轮子（类库），主要用于**加速后台C++跨平台开发**。里面的很多代码都是若干年前的了。代码的开发大部分利用的一些闲暇时间，当时就希望以后他能开源。很多年后，代码逐渐丰满。
为什么会有这个库呢，因为痒痒。痒了就要挠。其实很多代码可以用其他的库代替,但是他们有些不是太大太笨重，就是不具备可读性。一个轻巧的功能何必用数十万行代码实现?

ZCELIB运用的场景主要是后台开发。特别是游戏后台开发。

**跨平台代码只考虑两个平台,Windows(x86,x64)下的Visual C++, 和Linux下的GCC**。虽然我相信跨越其他平台也不是难事,但是那不是我写这套代码的目的.

## 为什么取名ZCELIB？

全称是**ZEN Communication Environment**，而为啥要有一个ZEN，这个问题其实很难回答，

大家如果有兴趣看过3D版本肉蒲团的英文名称就知道了，ZEN & SEX，其实如果说完全和自己无关那是骗人，但主要原因仍然是ZEN是禅的意思。TAO是道，期望自己的库名字酷一点。

### 比较完善的功能列表

ZCELIB是一个网络开放的基本类库，可以跨平台在Windows,Linux下使用。我自己主要在游戏业务下使用。其中包括各种服务器要用到的基本组件。包括网络，日志，加解密，MySQL封装，定时器，反应器，多线程封装，共享内存的各种封装，内存池，等。

已经比较齐全的功能

- **OS适配层**，为了适应WINDOWS和LINUX两个环境的代码增加的一层，同时都有一些扩展，基本可以替代ACE了。这部分其实用不少有趣的代码，主要思路是在Windows下模拟Posix的代码。让我的代码可以在Windows下直接调试。这部分代码都是以 zce\_os\_adapt\_XXX 开头命名的。代码都是在zce名字空间下，后面的函数名称和POSIX函数名称基本移植。。比较大块的功能包括，网络，时间，文件系统，GETOPT，C STRING的一些函数，线程，各种同步锁，共享内存的处理，进程和线程的监控。

- **LOG日志库和调试库**，以及一些调试的宏的封装,用于记录日志,可以定义分割日志的方式等(大小,日期),日志可以定级输出，输出点包括文件，stdout，stderr,Windows下的TRACE, 调试库,提供完善的调试方法,VC在这方面很强,偷学了一些.调试还可以输出当前的堆栈信息等。早起的日志类其实是对ACE日志的封装，但自己的代码要独立成行的时候还是把原来的那个改造了拿出来用了。另外提供一个完全模版化的日志输出接口。

- **TIME 时间和定时器**，原来打算只用 timeval，结果发现Time在网络编程中无处不在，还是有一个自己的封装比较容易控制，结果就有了ZCE\_Time\_Value这个非常底层的类，他内部就是 timeval，好在他在Visual C++也有，在OS适配层，就封装了各种系统的时间到timeval的转换。没有使用C++ 11的chrono的原因是在后台，C++和C的混用程度很高。所以timeval具有更大的可移植性。有了Time当然就要有定时器，我们的定时器是有2种封装，一种是比较常见的TimerHeap 定时器堆，一种是 TimerWheel 定时器时间轮，相对而言，TimerWheel具有更加强大的性能。

- **NET网络封装**，包括网络IPV4，IPV6的函数，OO的封装包括网络地址，UDP，TCP的封装，包括阻塞和非阻塞的发送，接收接口。连接器Connector和接收器Acceptor。可以大大方便你的开发。（本来这部分完全是使用ACE的，但最后还是去掉了ACE的部分代码，所以还是被迫自己实现了一个网络层。）

- **Reactor反应器**，包括select 的反应器，在Windows下和Linux下都可以使用，SELECT的反应器，epoll的反应器。

- **Thread线程封装**，线程包括OS层的封装，OO的封装包括线程对象，线程的管理器，线程之间的管道封装等。

- **MMAP STL**   游戏服务器GameServer，很多时候需要进行内存管理，同时希望崩溃后能立即恢复。所以就有了这套在共享内存里面实用的STL。（大部分接口类似，但实现还是完全不一样的。）对于MMAP STL的一些代码,我封装他们纯属无奈,其实我不认为使用MMAP有多大好处.我喜欢线程和普通内存,但是如果使用多进程的模型,而且要有一个持久存储时,你就必须用共享内存.而几乎所有的容器模板(包括ACE的)都使用了指针,而指针这种东西对于共享内存又是天敌.不过也好,可以通过写这些东西,彻底理解STL.(虽然我认定多线程的代码可以美丽优雅,决不逊色于多进程的各种模型,但是持久存储那是一堵墙)，而持久存储也许是一个好看但并不太实用的东西,恢复现场可能会有无数的问题.小心。

- **MySQL接口封装**，完整和方便是这个库最大的好处。OO封装分成connect，cmd，result几个部分封装，封装包装了重连，自动释放等功能。我在2005年甚至封装了STMT。前卫把。封装接口和原生接口类似,原生API是还不错，但还是有很多细节没有屏蔽。同时也有STMT的一组实用封装。但MySQL的STMT在多表的情况下实用并不方便，必须每个表保存一个STMT对象。（不如Oracle）所以原意实用的人不多。

- **SQLite**  SQLite 是一个好嵌入式数据库，做简单的配置管理是非常好的。

- **加解密**，字节处理封装，可以对数据量进行加密和解密。封装的加密算法很多。包括TEA，DES，DES3，XTEA，XXTEA，GOST，RC5，RC6，CAST5，CAST6，MARS，AES，在加上不同的块大小，KEY长度，轮数的定义，这些算法一起搞个协议栈对抗外挂不是什么难事把。，另外，对于流，提供了CRC的交织算法。

- Hash算法，包括MD5，SHA-1，CRC等。

- **快速压缩算法**，主要是对标LZ4进行了一个开发。LZ4的速度真TMD的快。我尝试努力逼近他。失败了。但我的代码的优势在于我在一些保护上做的更加完整一些。比较快速压缩如果用于协议。防止问题更加重要。

- **INI和XML配置文件读取**，会将配置读取到一个配置树上，另外，还有一个和Win32的API类似INI文件的读写,（不过也就是因为想和他的API类似,把代码搞复杂了），这块代码估计是这儿最老的代码，原来里面还有我当年的工号，9527，这是你的终生编号.

- **iNotify**   iNotify 机制是对于目录，文件发生变化是产生的事件进行回调处理（比如日志发生了改变）。对这个反应器进行了简单封装，目前甚至可以在Windows和Linux两个平台下使用，（当然底层反应器不一样，没辙）。


- MMAP  操作库,包括一些类似STL的模版。这部分算是当年的得意之作。主要用于在共享内存里面使用STL的语义东西。主要yuan'y

- SERVERKIT  服务器的工具类，目前提供了一个使用MMAP的状态统计类。

- PIDFILE    防止服务器多次启动的PID文件类，很有意思，发现UNIX通信卷2也讲过这些东西。土呀，还是土呀。

- RADOM 随机数的封装,这个代码其实和BOOST没有啥关系。是从GNU的数学库中慢慢扣出来的，而且我增加了一个薄薄的模板封装。从而让实用更加方便。

- Lua接口的封装，游戏中常见脚本。

### 未来可能会进一步完善的功能

- 前摄器的封装。
- CAS的Lock-free管道
- HTTP Client的封装？
- 一个类似BOOST的sigh的封装



## 如何编译

### Linux下的编译

Linux下的编译先要定义环境变量。

    export  ZCELIB_ROOT="/home/share/zcelib/zcelib.git"

我使用的环境是Ubuntu的14.04，GCC 4.8 以上版本，我自己最近使用的是Ubuntu 18.04 ,GCC 7.8。

然后先编译外部库，在depend目录下面直接有Makefile，可以直接使用，编译后的文件会放入lib目录，
其他ZCELiB的代码都提供Makefile，可以直接在各个目录下编译。工程的src目录下也提供Make所有代码的Makefile。可以直接使用。

一般的编译方法就是，其中make mkdir是用于建立相应的目录。

```shell
make mkdir
make 
```

如果需要debug（DEBUG=1）版本和realse（NDEBUG=1）版本。可以用下面的命令。

```shell
make debug=1 
make realse=1
```
关于Linux 下Makefile的说明，请参考**Linux下的Makefile编译** 

### Windows下的编译
Windows下的ZCE还是提供用VC++2013的工程，可以直接用来编译代码。编译后库文件放到lib里面，执行文件放入bin目录下面。其中VC++ 2013的工程的设置请参考**Visual Studio 工程说明**一节。

代码部分功能裁剪的调整在zce\_predefine.h这个预编译头文件里面。

### ZCELIB使用的第三方库说明：

ZCELIB依赖有一些一下外部库，详细请见后面的外部库说明。虽然我个人也推荐你自己下载各个库自己编译，但为了方便起见，我还是打包了外部库到目录里面。避免你们折腾。第三方代码都放在third_party目录下：

1. MySQL
2. RapidXML
3. SQLite
4. Protobuf
5. Lualib

对于Windows Visual studio，外部依赖的库包，里面的对于Windows的代码，我都提供了VC++ 2019的工程，你可以自己用VC++2019编译，会自动把文件放到lib目录下面。你可以任意编译X86，X64的版本。目录上都有区分。

而对于Linux，请选择GCC 4.8以上的版本作为编译器，在include目录下，有一个Makefile，直接make就会把几个依赖库的文件编译放到lib目录下，因为Linux系统的特点，我没有直接提供交叉编译的方法，比如在X64下没有提供编译x86的库的方式。

默认使用内部包含的第三方库。如果你需要自己管理，可以定义ZCELIB_THIRDPARTY环境变量。

```
export  ZCELIB_THIRDPARTY="/home/share/outer/third_party"
```

另外你可以在zce_predefine.h文件里面找到相应的**编译开关**关闭实用这些外部库，当然如果关闭了，对应的封装你也就无法实用了。
相应外部库包括头文件部分和库文件部分。头文件部分放在include目录下，库文件放在lib目录下，库文件目录安装Linux和Windows的做了细分。package目录用于存放各种安装包。

大致目录结构是：


    ├─install
    ├─include
    │  ├─lualib-5.1.5
    │  ├─mysql-linux-5.6.22
    │  ├─mysql-win-5.6.22
    │  ├─protobuf-2.6.1
    │  ├─rapidxml-1.13
    │  └─sqlite-3.8.8.1
    ├─lib
    │   ├─linux
    │   │  ├─x86
    │   │  └─x64
    │   ├─win
    │   │  ├─Win32-Debug
    │   │  ├─Win32-Release
    │   │  ├─x64-Debug
    │   │  └─x64-Release

#### MySQL

版本5.6.22
MySQL的头文件和库文件都来自MySQL的各种安装包提取出来的。
MySQL的头文件在Windows 下和Linux略有区别，所以头文件分成了2个目录存放。而相应的Windows Library，MySQL的安装包里面有2套。一套是static 库，一套是动态库，但static库在Visual Studio 2013下无法实用，会提示链接错误，只能实用动态库的版本。
Linux下，我们只使用相应的静态库。

MySQL的库文件在链接的时候会需要libz，如果不想使用这个版本，请自己安装一个。

    sudo apt-get install libz-dev

#### RepidXML

版本1.13
RepidXML的源码全部来自源安装包。
这个库用于解析XML文件，在配置读取等代码有实用。据说这个库的速度比tinyXML还要快，而且其只有头文件，不需要编译。这是我们选择他的原因。

#### SQLite

版本3.8.8.1
SQLite的源码全部来自源安装包。里面的shell.c是命令行工具的去掉了。
在include的目录下包括SQLite的所有源码，以及我增加相应的Visual Studio 2013的工程文件。编译后相应的static库文件会放入lib目录下对应的子目录。
Linux 相应的Makefile也是我根据自己的Makefile简化一个通用的。
注意SQLite实用GCC编译的。不是G++.

#### Protobuf

版本2.6.1
Protobuf的源码全部来自源安装包。
google本来就提供了工程文件，但为了编译的方便，我们升级到了Visual Studio 2013的版本，我还统一修改了输出目录的位置。
Linux下，Protobuf也有相应的AutoMakefile，编译会产生动态库和静态库，我们只使用相应的静态库。

未来是否支持PB 3.0 ，估计要看看心情。PB3.0 在很多游戏领域支持并不是特别好。

#### Lualib

版本5.1.5
在include的目录下包括Lualib的所有源码，以及我增加相应的Visual Studio 2013的工程文件。编译后相应的static库文件会放入lib目录下对应的子目录。
Linux下提供了Lualib相应的Makefile，Linux下，我们只使用相应的静态库。
Lua的编译有时候需要几个库，readline 和ncurses，可能要提前安装。

```shell
sudo apt-get install libreadline-gplv2-dev
sudo apt-get install libncurses5-dev
```

## Linux下的Makefile编译

先举一个我们代码里面的Makefile的例子，如下，使用GNU Make编译，虽然他看上去一点都不像Makefile。

```
#Richard Stallman说：软件就像性，免费的比较好.
#SAILLIB_ROOT必现提前定义，
#所有的工程下面的代码，可以使用相同的编译选项和方法，
#所有的预定义，编译方法，编译参数在src/make/makefile.define 
# src/make/makefile.linux src/make/makefile.rule 3个文件中都已经进行了
#预定义，你可以直接使用。
#如果你没有特殊需求，你就只用打开关闭下面定义就可以了。

#你要定义输出文件,和PRGNAME的名称这两个变量
#PRGNAME表示你的程序名称，一般就是你的目录名称，当然希望目录名称不要重复，
#OUTFILE,最总输出的文件名称，为啥要定义这个呢，最后删除的时候可以统一
PRGNAME = zergsvrd

#如果你要编译静态库 staticlib
#COMPILE_OBJECT = staticlib
#如果你要编译动态库
#COMPILE_OBJECT = dynamiclib
#如果你要编译可执行性程序
COMPILE_OBJECT = executeprg

#如果你使用预定头文件,请定义PREH_FILE,比如下面这样
#对应生成的GCH文件还只能放在当前目录下比较简单，放入其他目录很难保证其正常运行，打包源文件的脚本必须删除gch文件.
#如果你不适用预定头文件，可以屏蔽下面这个定义
PCH_FILE = zerg_predefine.h


#DEPLIB_DIR,标识编译当前的前提（依赖）目标目录  标识依赖目标的编译的库（目录）是什么，可以是多个
#如果依赖多个目录，请按照依赖的顺序填写，用空格隔开，
DEPENDS_DIR := $(ZCELIB_ROOT)/src/commlib/zcelib  $(ZCELIB_ROOT)/src/commlib/soarlib 

DEPENDS_LIB := $(ZCELIB_ROOT)/lib/libzcelib.a  $(ZCELIB_ROOT)/lib/libsoarlib.a

#这个里面定义所有的变量和定义，以及包含了平台性相关定义,你自己看吧
include $(ZCELIB_ROOT)/src/make/Makefile.define

#如果你想改变Mafile的规则，请在这个地方加入你自己的定义.

#这个定义其他所有的规则,还是你自己看吧
include $(ZCELIB_ROOT)/src/make/Makefile.rule
```

另外里面的注释应该比较清晰，请先阅读，里面其实东西不多。

PRGNAME 标识程序的名称。

COMPILE_OBJECT编译的对象是什么，有3个选择，staticlib静态库，dynamiclib动态库，executeprg为可执行程序。

PCH_FILE 为预定义头文件，为了加快编译的的东东，没有就注释掉。

DEPENDS_DIR，外部依赖的库文件目录，编译时，会扫描这些目录，重新进行makefile操作。 DEPENDS_DIR,标识编译当前的前提（依赖）目标目录 ，标识依赖目标的编译的库（目录）是什么，可以是多个，如果依赖多个目录，请按照依赖的顺序填写，用空格隔开。

DEPENDS_LIB，外部依赖的文件。如果有更新，会重新编译文件。

上面这个例子中，这样会编译一个名字为zergsvrd的可执行程序。他的预编译头文件是zerg\_predefine.h。他会使用目录下的所有的CPP文件生成这个文件。他的外部依赖的目录和库文件也在DEPENDS\_DIR，DEPENDS\_LIB下有定义。当这些目录更新的时候，zergsvrd也会重新编译。

其他相应的规则，写在两个文件里面。

$(ZCELIB_ROOT)/src/make/Makefile.define   这个里面定义所有的变量和定义，以及包含了平台性相关定义,你自己看吧

$(ZCELIB_ROOT)/src/make/Makefile.rule，这个定义其他所有的规则,还是你自己看吧，如果你想改变Mafile的规则，请在这个定义前面加入。你自己的定义.



编译从此变得快捷，安全，爽！

## Makefile头文件

对的，ZCELIB的Makefile爽歪歪所依靠的都是，这个目录下的， 3个Makefile的头文件。这个3个文件内部都有非常非常详细的注释，我不再这儿费太多的口舌了。

- Makefile.define 定义ZCELIB 内部的各种变量，工程目录,需要链接的各种外部库，内部库等。头文件目录，包括外部和COMMLIB的各种目录。如果你有特殊的Make需求，可以修改这个文件。
- Makefile.linux 定义Linux环境下的各种环境变量，编译选项，很多选项在这个地方都有定义，比如你想编译debug版本，就make debug=1就可以了。
- Makefile.rule 所有编译规则的核心定义，非常重要的一个地方。包括内部的预编译头文件，SO，STATIC Library，EXE，的编译规则，内部依赖关系的.d文件的实用，甚至包括外部依赖关系的处理。都在这儿了。

目前我仍然没有使用CMake，大约是我曾经为如何写好Makefile作出过巨大努力。不愿意

## Visual Studio 工程说明

原来ZCELIB对Visual Studio的版本兼容做了很多努力，甚至可以使用2003的Visual C++编译，但这实在不是什么让人开心的事情，而且我喜欢C+11的部分特性，我干脆懒到低，只支持Visual C++ 2013以后版本了。好处是显而易见的。坏处是如果你要移植会烦恼不少。而且工程文件升级挺容易，降级就……，算了，说的好像有多少人会看你的代码一样。

Visual Studio的版本库管理其实挺麻烦的，（其实Linux下也不容易）。各种Visual Studio C运行时库之间的兼容性都不是特别好。所以**我们坚持对不同的版本，把库文件放到不同的目录**，而不是通过文件名称带d，之类的方法区分。熟悉Windows下开发的人会明白我的，这样能省略很很多麻烦。在工程设置上也会简单很多。

工程文件内部的设置如下,对于各种Configuration 和 Platform（填写时，Configuration 可以选择 All Configurations Platform 可以选择 All Platforms ），我们的配置都是一致的，参考如下：

- Output Directory  即 $(OutDir) 填写  $(SolutionDir)../lib/$(Platform)-$(Configuration)/
- Intermediate Directory 填写  $(SolutionDir)../obj/$(Platform)-$(Configuration)/$(ProjectName)/
- Character Set 填写  Not Set
- TargetName 选择  $(ProjectName)
- Output File 填写  $(OutDir)$(TargetName)$(TargetExt)

如果外部需要包含我们的库，目录配置上这样写，，引入库的文件名称也可以填写成一样。省事很多，也容易找到问题。
-Additional Libary Directories可以填写为 $(SolutionDir)../lib/$(Platform)-$(Configuration)/

解释一下一些关键字。
- $(Platform)是Win32或者x64
- $(PlatformToolset)，如果需要多个VS版本同时存在（这种倒霉项目不少），请在前面的目录里面增加$(PlatformToolset)。目前VS2019是v142，也就是Visual Studio 2019的版本号。大致是v140是2017，v130是2015，v120是2014，v110是2012，v100是2010，v90是2008，v80是2005，v71是2003，v70是是2002，v60是大名鼎鼎的Visaul Studio 6.0。为啥我这么熟悉？我都用过。
- $(Configuration) 是Debug或者Release。


注意Runtime Library和Character Set的填写。这个地方在Visual Studio的链接的时候是非常容易惹麻烦的。Runtime Library我们选择
- Debug版本的Runtime Library 选择  Multi-threaded Debug DLL (/MDd)
- Realse版本的Runtime Library 选择  Multi-threaded DLL (/MD)
如果你有特殊需求请自己调整。

## 造轮子的故事和我喜欢的C++
我不是一个非常喜欢造轮子的人,但我也忍受不了很多很臭的袜子。
我对于类库是非常挑剔的.简单,优美,稳定,效率高,C++对象化的我才看得上眼.
STL(STLPort),ACE,Boost,才是我看得上的类库.如果一个库实在违反我的代码美学,我宁愿自己写一个.

C++ 是难学易用的东西,不过我建议你掌握.可惜大部分的自称C++的程序员只停留在认为C++是带类的C的阶段

对于说STL,效率低的人,我报以同情.建议你把认为效率低的代码给我重写一下.赫赫.
STL(SGI)那是世界上最聪明的几个脑瓜程序员实现的东西,诬蔑之前掂量一下.


## 必须感谢的人，排名不分先后，
先后有很多兄弟们帮助我完善了这个库，特别是11年的改写后，由于时间不足，大量的测试工作是他们完成的。

- scottxu, 很早贡献过一套共享内存红黑树代码
- inmoreliu,linkerlin，帮助打过酱油，
- sonicmao，rong，他们的部分代码我都参考过。
- chrislin，贡献过很多类似boost的代码
- chweiling，errayzhao，djiang，liangfeng，sasukeliu，stefzhou，yunfeiyang，pascalshen平台开发组的兄弟们，2011年的版本他们贡献良多，这套代码能在几万台机器上一起跑。感谢他们。
- yunfeiyang 在2011-2012一段时间一直在帮忙增加代码和帮忙改写我这个流氓种下的bug，
- sriverxiao 帮我测试过多节hash

## 编程的乐趣
以下是《Learn Python The Hard Way, 2nd Edition》这本书的尾声部分。摘录下来给大家看看。

看完了这本书，你决定继续做编程。也许它能成为你的一个职业，也许它能成为你的一项爱好。但你需要一些指导，确保自己不会走错了道路，或帮助你从这个新业余爱好中得到最大的乐趣。

我做了很久的编程。久的你都想象不出来，久的都让我苦恼。就在我写这本书的时候，我大概懂20种编程语言，而且我可以用一天或长点儿用一周的时间学会一种新语言——要依这种语言有多奇怪而定。但这最终成为了我的苦恼，它们已经不能再吸引我的兴趣。我并不是说这些语言没有意思，或告诉你你会觉得它们很枯燥。只是想说在我的职业旅程走到现在，我已不再对语言有兴趣。

经过这么多年的学习经历，我发现语言本身并不重要，重要的是你如何用它们。事实上，我一直知道这个道理，但我总是被语言吸引走，周期性的忘记这个道理。现在我不再忘记了，你也应该这样。

你会什么语言、你用什么语言，这并不重要。不要被围绕在编程语言周围的各种宗教宣传迷惑，那些只会遮蔽你的眼睛，让你看不出这些语言只是一种让你做有趣的事情的工具而已。这才是它们的真正属性。

编程作为一种智力活动，它是唯一的一种能让你创造出交互式艺术作品的艺术形式。你创造出来人们可以操作的软件，你是在间接的和人们交互。没有任何其它艺术形式有如此的交互性。电影是单向的向观众传输信息。绘画是静态的。而软件程序却是双向动态的。
编程只能算是一项一般有趣的工作。它可以成为一个不错的职业，但如果你既想多挣钱又要干的高兴，不如去开一家快餐馆。如果你把编程当做一种秘密武器在其它行业里使用，也许会有更好的效果。

科技界科技公司里会编程的人多如牛毛，没人会在意他们。而在生物界，医药界，政府，社会学界，物理界，历史界和数学界，如果你有这种技能，你能做出令人瞩目的事情。

当然，所有的这些话都是没有意义的。如果通过这本书，你喜欢上了编程，你应该尽你最大的努力，通过它来改善你的生活。去探索这神奇的精彩的智力活动，也只有近50年来的人有机会从事这种职业。如果你喜欢它，就尽情的热爱它吧。

最后我要说的是，学习开发软件会改变你，让你与众不同。不论是好的不同还是坏的，反正是不同。你会发现，因为你会开发软件，人们会对你很冷淡，会用“书呆子”这样的词形容你。你会发现，由于你善于剖析逻辑，人们痛恨跟你辩论。你甚至会发现，只是简单的懂一些计算机原理都会给你带来很多烦恼，让你跟他们比起来怪怪的。

对于这些问题，我只有一点小建议：让他们去死吧。这世界需要更多的能知道事情如何工作、喜欢去探索的人。当他们这样对待你时，你要记住，这是你的生活，不是他们的。与众不同不是罪恶，人们这样对你只是出于嫉妒，嫉妒你拥有一项他们在梦中都无法拥有的技能。
**你会编程。他们不会。这真他妈的酷毙了。**

## 修改记录，应该说是ZCELIB日记

- 20060101
  越来越和该死的进程，线程打交道,感觉要放弃原来考虑的不合ACE打交道
  如果是一个的后台的库,不用ACE感觉基本不可能。犹豫

- 20060102
  今天感觉日志类不完善,没有线程部分保护的东西,由于内部有自己的缓冲去,在多线程的情况下肯定会出现问题，去看了看ACE的策略日志类，也就研究了一下ACE的日志部分。感觉ACE的封装也一般般,日志策略全部使用命令行作为初始化方式,我没有这个爱好。另外所有的时间戳也是调用API得到的,这个这个，其实查询时间的API是个不是个高效操作.kingwang的评价有道理,ace压根就没想非常高效。只是一般高效。呵呵
  不过还是按照自己的感觉封装一个ACE的日志策略类,完了,既然打算用ACE，就抛弃那些自己写的跨平台的代码把。

- 20060315
  看了C4A的服务器统计类，(rong,sonicmao)觉得是一个非常好的想法，所以偷师了。

- 20060610
  加入了一个MYSQLProcess类,其实这个封装我的相当多的代码用过,不过为了在CGI的代码中间
  偷懒,我都是每个LIB写一个类似的,
  这次完整的封装一下,以后要改变写类似代码的习惯.
  另外,决定将代码统一改为ACE UNIX风格.和bolanfan PK了一下午,虽然我们俩说的问题都是不是一个层面,
  我还是认可统一规范的重要的,

- 20070904
  好久没有改这个库了，前不久加入了一个PID文件的输出类，加了文件锁。终于看完了UNIX的记录锁
  。也算有点收获，小小进步一下。

- 20110320 到了一个一个新部门，要为整个部门的机器部署游戏安全模块。决定重构这个库，打算摆脱ACE，一方面是liangfeng不喜欢，一方面是我自己也觉得ACE有点重了，本来打算投奔Boost，但是使用Boost的过程发现Boost离我心中的目标还非常遥远，我只有放弃了Boost。打算自己写。
  所以改动很多，包括增加OS适配层，重新改写日志模块，重新整理了大量代码。这个库目前的基本框架都是这个时候定型的。

- 20111030
  大规模改写代码大致完成，改写断断续续，中间耗费时间持续了大约2个月，大部分是周末时间，特别是那个苦难的10.1长假，不过速度也算飞快。
  但是bug还是很多，yunfeiyang，liangfeng,stevzhou,erray，佐助，等朋友都帮我修改了这套库，感谢他们帮我调试。

- 2012年5月7日
  修改了所有的卫哨定义。

- 2012年6月
  zen_server_status监控的代码进行了改写，不再使用数组ID作为监控下标，而使用hash table作为idx，性能有所降低，但兼容性要好很多。
  djiang PK胜利。

- 201208
  对于原来的依靠模版完成GUARD功能进行了反思，增加利用OO完成的锁功能。

- 20120920
  这一年都在小规模的改写这个库，有点味道了，至少像那么回事了。
  9月BB出世，在陪产假期继续完善了一下库。改写部分Mutex部分，增加了一些新代码，比如SPIN的锁，Process的几个锁的封装。
  增加了IPV6的支持，（虽然增加了，不知道什么时候才能测试）
  增加了一些域名相关的代码。
  增加了SYSTEM V的共享内存的跨平台支持，

- 201210
  增加了random的代码。增加了uuid的代码。 是不是应该增加一个OPENID的生成函数。哈哈。

- 20121108
  今天中国著名的斯巴达召开了。哦喔喔。
  增加了一些读取本机配置性能，进程性能的代码。
  这些代码会增加我们监控的效果。
  pascalshen帮助我修正了很多错误。

- 20130101
  离开了原来的部门，不再和原来公司的代码同步，保持自己的代码的唯一性。也准备上github了。
  主要为了避免未知的版权风险，把代码彻底独立独立出来，改名。

- 20141001
  这一年都在胡乱的增加一些新功能，包括Lua的结合代码，对MPL有了一点点最初步的折腾。
  初步尝试一些C++ 11的东西。
  把原来的commlib，commsvr的东西正理了出来，放入代码库。
  对协程和状态机的异步框架做了一次封装。

- 20150101 GCC4.8 下面所有的代码第一次编译通过，GCC真的太严格了。一个后台程序员居然曾也要记录一下。

- 20190425 整理代码到VS2019。然后测试了一下代码分析功能。发现。。。真能帮忙整理出来不少代码错误。
