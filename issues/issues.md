# issue


#### 植入右键菜单功能

#### Windows二维码库的选择


C语言二维码的lib库使用libqrencode,该库来自FUKUCHI Kentaro，用于由QRCode文字生成bmp文件。  
项目地址为：[libqrencode的github地址][]

#### Windows编译libqrencode

1. libqrencode版本：libqrencode 3.4.4 
2. 编译方法可以查看：[VS2015编译qrencode-3.4.4][], VS2010也可以编译出libqrencode的静态库，会出现下图的错误信息  
![编译错误图片][]  
解决的方法：[error C2054: 在“inline”之后应输入“(”][]
3. [libqrencode学习笔记（二）： 用libqrencode静态库库生成二维码并保存为BMP图片][]

#### 修改注册表，添加用白名单

通过对注册表设置，向注册表的KEY: **HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Terminal Server\TSAppAllowList**添加subkey,来增加远程可以打开的应用白名单.

[Geekswithblogs.net][]上的相关回答，不过现在这个网站访问不了  
[archive.io][]上有也有相关的回答：

> Some tips from my fiddling.
>
> 1. do not set fDisableAllowList=1 or else you lose any control on what you allow remote users to run. (eg. remoteapplicationprogram:s:even_a_direct_path_to_any_exe_will_do)
>
> 2. use a double pipe before the key name on the rdp file (eg. remoteapplicationprogram:s:||1234567)
>
> 3. the step
"Create New -> String Value, Name. Set value to Notepad"
is not required. "Path" is the only one really needed. Resuming, remoteapplicationprogram will cause a lookup for "||" + any keyname inside TSAppAllowList\Applications, and run whatever is pointed by it's "Path".

下图是把notepad设置为RemoteApp可使用的应用白名单：  
![注册表图片][]

#### 异常处理

#### 自动识别应用路径并添加进注册表表白名单

找到系统安装的office路径和pdf预览程序的路径，添加进白名单;

#### 对不支持的文件类型进行提醒

当所选文件类型不是我们分享支持的文件类型时，需要提醒用户，这个文件不支持生成分享二维码功能，client程序进行处理；

#### 只针对特定文件类型才可以生成二维码
针对第6点，是不是可以找到对特定类型的文件才可以产生分享二维码？这样可以直接修改注册表完成这个功能，不需要client程序处理；

#### remoteapp在192.168.1.234下没有响应

192.168.1.234 机器为Windows 8.1旗舰版

#### MSDN上关于remote desktop client 的一些FQA

[FQA about Remote Desktop Client][]

#### 深入调研远程连接桌面的设置

#### 深入调研remoteapp的设置
在192.168.1.114机器（Windows 7旗舰版 破解版）上，远程桌面登录后，打开remoteapp会提示**出现内部错误**,
反之亦然

但在61.146.164.120:10325机器（Windows 7旗舰版 为破解版）上，远程桌面登录后，可以打开remoteapp，反之亦然

需要找出两台机器的差异。

#### 上传文件接口调用


#### MsTscAx class的调用
这样可以更好地对远程连接桌面和远程连接应用进行异常处理，包括并不限于：  
1. 连接建立了，但UI没有出来（现阶段只要侦测到TCP连接建立就认为远程桌面打开完成，实则还有一些UI处理完后才被认为远程桌面连接完成）；
2. remoteapp打开远程文件时，文件不存在；
3. remoteapp或者mstsc打开远程桌面时，由于远程桌面的已经达到配置连接数的极限，所以无法打开；


#### 文件分享的基本流程
1. 右键生成二维码（用户名 + 域 + 文件路径）
2. 小智通过快捷键方式触发事件，获取当前活动云桌面的IP和端口；
3. 小智解释出当前二维码的信息；
4. 小智把二维码信息 + IP + 端口，发送给当前与小智绑定的小慧A；
5. 小慧A把第4点获取到的信息推送给小慧B；

前提，统一时间内只有一台小慧使用小智












[Geekswithblogs.net]:http://geekswithblogs.net/twickers/archive/2009/12/18/137048.aspx
[archive.io]:http://archive.is/abBF
[注册表图片]:/D:\lf\cloud-desktop-client\issues\res\1.PNG/
[FQA about Remote Desktop Client]:https://docs.microsoft.com/en-us/windows-server/remote/remote-desktop-services/clients/remote-desktop-client-faq
[libqrencode的github地址]:https://github.com/fukuchi/libqrencode
[VS2015编译qrencode-3.4.4]:http://blog.csdn.net/ljttianqin/article/details/73699777
[编译错误图片]:/D:\lf\cloud-desktop-client\issues\res\compile_error.PNG/
[error C2054: 在“inline”之后应输入“(”]:http://www.cnblogs.com/lgh1992314/p/5834697.html
[libqrencode学习笔记（二）： 用libqrencode静态库库生成二维码并保存为BMP图片]:http://blog.csdn.net/ljttianqin/article/details/73823666