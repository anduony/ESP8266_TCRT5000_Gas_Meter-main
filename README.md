# ESP8266_TCRT5000_Gas_Meter

本项目主要实现如下功能：将现有燃气表中的数据更新到Homeassistant中，实现可以看到燃气表在各个时段，每天，每周，每月的使用量。

### 1. 功能原理
![image.png](https://raw.gitcode.com/anduony/ESP8266_TCRT5000_Gas_Meter/attachment/uploads/2c88b767-95ed-4c73-a098-b0833c107005/image.png 'image.png')

现有燃气表中机械码盘最后一位有的数字上会有一个金属片，使用这个金属片对红外寻迹传感器进行反光，使得红外传感器信号发生变化，那么就可以发送一个信号到服务器，服务器就可以根据这个信号进行计数。这里传输数据使用MQTT进行传输。数据展示使用Homeassistant。

### 2. 元器件

TCRT5000寻迹红外传感器**2元包邮

![1741418280346.png](https://raw.gitcode.com/anduony/ESP8266_TCRT5000_Gas_Meter/attachment/uploads/57a46ead-47d9-4dbd-8d46-7d6a09049332/1741418280346.png '1741418280346.png')

[TCRT5000购买链接](https://detail.tmall.com/item.htm?_u=84rdt91ce63&id=601333318777&pisk=gSEUsaxkmMIes9241omy73wNPjiKc05fZuGSE82odXchyvsk48ymF9CRpPkuN5BRpDwoa7Ps63M5J_Kz_Ww0R2N7RbDos52BAbsKz7ybM7t5vyMk45w-r7Zze3kuE7BKN9Q_pJn-qs1j4iwLpBRKIxqexhYgBxDktNgH8GyCCs1fcgrs1155GWNTlMSi3xmoK4DojOkSUQDuZ2vMjfH-EBDkxOWZefDHZ4DuIdD-Lvv3ZY2ijYDJxH0HqCAinfcoq4VuIOkSGmruYhlE-ToGZu_yPDuEiJcw0REZKalVDfxlNlDUQjwhy3xubvuU2E3ZjDuQr-3YAJsyXDw4ScDUZifnj-4ah2rFgHG-zlPUzW6WyAzzEonK0Lj3QDlEokupeHF0Q84j7l6cDDontziLFK5QQkPQplyWEFmEAPu4Ymjvtba_3ukz2_IagJ40ZzSzhQHGorq88QYr-AHZGOWNLAv0TuTRnFLJyVXtQj6V3UL--AHZGOWwyU3g6Alf3t5..&spm=a1z09.2.0.0.61d52e8djLXxmA)

ESP8266开发版**10元包邮

![1741419283817.png](https://raw.gitcode.com/anduony/ESP8266_TCRT5000_Gas_Meter/attachment/uploads/c969eba3-f982-4e46-8394-bc2655387079/1741419283817.png '1741419283817.png')

### 3.硬件连接
首先看开发板的脚位图如下：
![image.png](https://raw.gitcode.com/anduony/ESP8266_TCRT5000_Gas_Meter/attachment/uploads/9a587700-5bf3-4fa5-acf5-96807f0f3db2/image.png 'image.png')

### 4.MQTT服务申请
MTQQ服务可以自己搭建，我这里服务器版本问题，没有自己安装，找了一个免费版的，足够这个传感器使用。
MTQQ地址：https://cloud.emqx.com/
申请成功之后部署一个服务

![image.png](https://raw.gitcode.com/anduony/ESP8266_TCRT5000_Gas_Meter/attachment/uploads/13ca9ab6-c24c-41f1-8316-f82b1e0bc280/image.png 'image.png')

申请成功之后有如下页面红框中标注是重点关注的地方

![1741420228922.png](https://raw.gitcode.com/anduony/ESP8266_TCRT5000_Gas_Meter/attachment/uploads/1dbf4b8d-6479-4fdb-944a-ab7cf9e601ab/1741420228922.png '1741420228922.png')

### 5.ESP8266代码
我这边使用arduino进行编写，

#### 首先要加载ESP8266的库；
在文件->首选项 如下位置填写如下地址：https://arduino.esp8266.com/stable/package_esp8266com_index.json

![image.png](https://raw.gitcode.com/anduony/ESP8266_TCRT5000_Gas_Meter/attachment/uploads/f9f76b6c-b891-42e8-b6e9-36a82b34c885/image.png 'image.png')
按照如下位置下载ESP8266的库

![image.png](https://raw.gitcode.com/anduony/ESP8266_TCRT5000_Gas_Meter/attachment/uploads/e123e5c4-3790-4b93-839c-e5f9800bf355/image.png 'image.png')
板子类型选择

![image.png](https://raw.gitcode.com/anduony/ESP8266_TCRT5000_Gas_Meter/attachment/uploads/c2d7e876-b7d8-4885-a344-58f7e4e2e3a4/image.png 'image.png')

#### 安装MQTT的依赖
安装 PubSub client 库。 点击项目 -> 加载库 -> 管理库...。搜索 PubSubClient，安装 PubSubClient by Nick O’Leary。

![image.png](https://raw.gitcode.com/anduony/ESP8266_TCRT5000_Gas_Meter/attachment/uploads/8d795c86-dd67-4c0c-b6c9-236b49a34094/image.png 'image.png')

#### 代码修改
![image.png](https://raw.gitcode.com/anduony/ESP8266_TCRT5000_Gas_Meter/attachment/uploads/bf204686-4f07-4f83-8349-a3bf2a020c2f/image.png 'image.png')

第一处修改就填写MQTT上面的服务器地址

第二处修改填写账号密码（从下面的位置获得账号密码）

在MQTT服务器位置如下地方设置账号密码

![image.png](https://raw.gitcode.com/anduony/ESP8266_TCRT5000_Gas_Meter/attachment/uploads/ac0ac1c7-9681-4f86-aee7-5393139f4d00/image.png 'image.png')

第三处修改密钥：从服务器信息也下载CA证书打开复制到下面。

第四处修改你的wifi密码

最后就是编译下载代码到ESP8266中

### 6.homeassistant中配置
第一首先打开高级模式，步骤如下

![image.png](https://raw.gitcode.com/anduony/ESP8266_TCRT5000_Gas_Meter/attachment/uploads/f33342f0-9b56-4e15-bb6e-901be00104d3/image.png 'image.png')

第二步配置MTQQ
配置-->设备与服务-->添加集成  搜索 MTQQ 点击第一个开始配置，填写后打开高级进行配置，配置信息如下：

![image.png](https://raw.gitcode.com/anduony/ESP8266_TCRT5000_Gas_Meter/attachment/uploads/fb7dfe0d-3405-4d2b-8cdf-169f42ba2bbb/image.png 'image.png')

第三步开始测试通讯：如果你的8266上传了上面的代码，并连上网络。你通过遮挡红外传感器，这个地方就会收到信息。

![image.png](https://raw.gitcode.com/anduony/ESP8266_TCRT5000_Gas_Meter/attachment/uploads/98c91e7e-e9a5-4e4d-9283-e92af6d655d5/image.png 'image.png')

第四步配置homeassistant的configuration.yaml文件
复制代码文件到上面的configuration.yaml中即可，这个代码是deepseek自动生成，有一些内容虽然没用，但是缺了HA又不会工作，有大神可以帮修改优化。
第五步添加卡片，这个地方就不多说了。

![image.png](https://raw.gitcode.com/anduony/ESP8266_TCRT5000_Gas_Meter/attachment/uploads/17a995e4-f7a7-4ea3-b16f-deaa5ddfb897/image.png 'image.png')

### 7.传感器调试

首先将所需要的设备全部烧写完成就可以去表上调试，调试方法：把表首先固定在对应的位置，然后转动蓝色变阻器使得左边的LED不亮，然后停下来，观察当反光片过来的时候是否变亮，如果变亮证明正好，否则在这个附件调节。最终实现有反光片左侧的LED亮，没有反光片的时候不亮就调节好了。

![image.png](https://raw.gitcode.com/anduony/ESP8266_TCRT5000_Gas_Meter/attachment/uploads/054f04b9-e1e7-47b6-bb20-ca5ef042734d/image.png 'image.png')

### 8.最终成果
![image.png](https://raw.gitcode.com/anduony/ESP8266_TCRT5000_Gas_Meter/attachment/uploads/f40b9790-3466-4e3e-ab75-86308304e6f8/image.png 'image.png')

