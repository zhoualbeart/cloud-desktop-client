# issue

1. 植入右键菜单功能；
2. 生成二维码；
3. 修改注册表，添加远程可打开应用白名单；

    通过对注册表设置，向注册表的KEY: **HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Terminal Server\TSAppAllowList**添加subkey,来增加远程可以打开的应用白名单.

    [Geekswithblogs.net]][]上的相关回答，不过现在这个网站好像访问不了  
    [archive.io]][]上有也有相关的回答：

    > Some tips from my fiddling.
    >
    > First, do not set fDisableAllowList=1 or else you lose any control on what you allow remote users to run. (eg. remoteapplicationprogram:s:even_a_direct_path_to_any_exe_will_do)
    >
    > Second, use a double pipe before the key name on the rdp file (eg. remoteapplicationprogram:s:||1234567)
    >
    > Last, the step
    "Create New -> String Value, Name. Set value to Notepad"
    is not required. "Path" is the only one really needed. Resuming, remoteapplicationprogram will cause a lookup for "||" + any keyname inside TSAppAllowList\Applications, and run whatever is pointed by it's "Path".

    下图是把notepad设置为RemoteApp可使用的应用白名单：  
    ![注册表图片][]


4. 异常处理；
5. 找到系统安装的office路径和pdf预览程序的路径，添加进白名单;
6. 当所选文件类型不是我们分享支持的文件类型时，需要提醒用户，这个文件不支持生成分享二维码功能，client程序进行处理；
7. 针对第6点，是不是可以找到对特定类型的文件才可以产生分享二维码？这样可以直接修改注册表完成这个功能，不需要client程序处理；
8. remoteapp在192.168.1.234下没有响应；
9. MSDN上关于remote desktop client 的一些FQA

    [FQA about Remote Desktop Client][]













[Geekswithblogs.net]:http://geekswithblogs.net/twickers/archive/2009/12/18/137048.aspx
[archive.io]:http://archive.is/abBF
[注册表图片]:/D:\lf\cloud-desktop-client\issues\res\1.PNG/
[FQA about Remote Desktop Client]:https://docs.microsoft.com/en-us/windows-server/remote/remote-desktop-services/clients/remote-desktop-client-faq