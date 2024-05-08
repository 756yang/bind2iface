
# bind2iface

这是弯路执行`Winsock API`的工具，以使几乎所有网络流量绑定环境变量指定的接口。

根据[Detours](https://github.com/microsoft/Detours)官方配置好编译环境，你需要\
安装新版`Virtual Studio`(旧版可能不支持)，注意，建议选择`.NET桌面开發`和\
`使用C++的桌面开發`工作负载，以确保Detours可正确编译。

打开`x64本机命令行提示符工具`，并切换工作目录到源码路径，可执行以下操作：

编译源码：

	nmake

测试源码：

	nmake test

清除编译目标：

	nmake clean

生成目标文件`bind2iface64.dll`是我们所需的工具，使用方法参考：

``` bat
set SOCK_BIND_IFACE_INDEX=-1
set SOCK_BIND_IFACE_INDEX_V6=-1
.\lib.X64\withdll.exe -d:.\bin.X64\bind2iface64.dll .\bin.X64\test.exe www.baidu.com 80
```

其中，`SOCK_BIND_IFACE_INDEX`是IPv4绑定的接口序号，`SOCK_BIND_IFACE_INDEX_V6`\
是IPv6绑定的接口序号，值`-1`让`bind2iface64.dll`决定绑定的接口，值`0`允许所有\
接口(等同于`Winsock API`的默认行为)，无效值不改变使用的接口。

## bind2iface (English)

This is a detour implementation of the `Winsock API` tool to enable almost
all network traffic to be bound to the interface specified by environment
variables.

According to the official configuration of [Detours](https://github.com/microsoft/Detours),
you need to install a new version of `Virtual Studio` (the old version may not
be supported). Note that it is recommended to choose `.NET Desktop Development`
and `Desktop Development with C++` workload to ensure Detours compiles correctly.

Open the `x64 native command line prompt tool` and switch the working directory
to the source code path. You can perform the following operations:

Compile source code:

	nmake

Test source code:

	nmake test

Clear compilation target:

	nmake clean

Generating the target file `bind2iface64.dll` is the tool we need. Please refer
to the usage method:

``` bat
set SOCK_BIND_IFACE_INDEX=-1
set SOCK_BIND_IFACE_INDEX_V6=-1
.\lib.X64\withdll.exe -d:.\bin.X64\bind2iface64.dll .\bin.X64\test.exe www.baidu.com 80
```

Among them, `SOCK_BIND_IFACE_INDEX` is the interface serial number bound to
IPv4, `SOCK_BIND_IFACE_INDEX_V6` is the interface serial number bound to IPv6.
The value `-1` allows `bind2iface64.dll` to determine the bound interface.
The value `0` allows all interface (equivalent to the default behavior of
the `Winsock API`), invalid values do not change the interface used.
