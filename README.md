# ESP8266_TCRT5000_Gas_Meter

本项目主要实现如下功能：将现有燃气表中的数据更新到Homeassistant中，实现可以看到燃气表在各个时段，每天，每周，每月的使用量。

### 1. 功能原理
![image](https://github.com/user-attachments/assets/ef9ff95a-428a-433e-b297-657edec3426c)


现有燃气表中机械码盘最后一位有的数字上会有一个金属片，使用这个金属片对红外寻迹传感器进行反光，使得红外传感器信号发生变化，那么就可以发送一个信号到服务器，服务器就可以根据这个信号进行计数。这里传输数据使用MQTT进行传输。数据展示使用Homeassistant。

### 2. 元器件

TCRT5000寻迹红外传感器**2元包邮

![image](https://github.com/user-attachments/assets/69660fcf-b1b9-46bc-bd11-9521f1797b11)


[TCRT5000购买链接](https://detail.tmall.com/item.htm?_u=84rdt91ce63&id=601333318777&pisk=gSEUsaxkmMIes9241omy73wNPjiKc05fZuGSE82odXchyvsk48ymF9CRpPkuN5BRpDwoa7Ps63M5J_Kz_Ww0R2N7RbDos52BAbsKz7ybM7t5vyMk45w-r7Zze3kuE7BKN9Q_pJn-qs1j4iwLpBRKIxqexhYgBxDktNgH8GyCCs1fcgrs1155GWNTlMSi3xmoK4DojOkSUQDuZ2vMjfH-EBDkxOWZefDHZ4DuIdD-Lvv3ZY2ijYDJxH0HqCAinfcoq4VuIOkSGmruYhlE-ToGZu_yPDuEiJcw0REZKalVDfxlNlDUQjwhy3xubvuU2E3ZjDuQr-3YAJsyXDw4ScDUZifnj-4ah2rFgHG-zlPUzW6WyAzzEonK0Lj3QDlEokupeHF0Q84j7l6cDDontziLFK5QQkPQplyWEFmEAPu4Ymjvtba_3ukz2_IagJ40ZzSzhQHGorq88QYr-AHZGOWNLAv0TuTRnFLJyVXtQj6V3UL--AHZGOWwyU3g6Alf3t5..&spm=a1z09.2.0.0.61d52e8djLXxmA)

ESP8266开发版**10元包邮

![image](https://github.com/user-attachments/assets/cefeb193-e9a7-4fef-b457-dd0fb484a3ca)


### 3.硬件连接
首先看开发板的脚位图如下：
![image](https://github.com/user-attachments/assets/31e55ee2-86eb-4e92-81ea-717aad625fa8)


### 4.MQTT服务申请
MTQQ服务可以自己搭建，我这里服务器版本问题，没有自己安装，找了一个免费版的，足够这个传感器使用。
MTQQ地址：https://cloud.emqx.com/
申请成功之后部署一个服务

![image](https://github.com/user-attachments/assets/da8cb3e9-2a35-4c68-aa3c-a7260e8d810e)

申请成功之后有如下页面红框中标注是重点关注的地方

![image](https://github.com/user-attachments/assets/7f636895-031b-4c4c-bad7-e4c24278fd5c)


### 5.ESP8266代码
我这边使用arduino进行编写，

#### 首先要加载ESP8266的库；
在文件->首选项 如下位置填写如下地址：https://arduino.esp8266.com/stable/package_esp8266com_index.json

![image](https://github.com/user-attachments/assets/a34050d7-cd4b-4569-a185-e938a2e43059)

按照如下位置下载ESP8266的库

![image](https://github.com/user-attachments/assets/eb23ce7b-fd33-43fd-9bfb-ddb517143487)

板子类型选择

![image](https://github.com/user-attachments/assets/653d8ae8-1dc3-4145-b964-7aa36891c278)


#### 安装MQTT的依赖
安装 PubSub client 库。 点击项目 -> 加载库 -> 管理库...。搜索 PubSubClient，安装 PubSubClient by Nick O’Leary。

![image](https://github.com/user-attachments/assets/6842c1b4-2748-4f8f-a44c-523477482e2c)


#### 代码修改
![image](https://github.com/user-attachments/assets/523d4cf0-4565-4a62-aa0f-141a1f512cd1)


第一处修改就填写MQTT上面的服务器地址

第二处修改填写账号密码（从下面的位置获得账号密码）

在MQTT服务器位置如下地方设置账号密码

![image](https://github.com/user-attachments/assets/07407396-bfcd-4f22-bf58-76d48f16c7f0)


第三处修改密钥：从服务器信息也下载CA证书打开复制到下面。

第四处修改你的wifi密码

最后就是编译下载代码到ESP8266中

### 6.homeassistant中配置
第一首先打开高级模式，步骤如下

![image](https://github.com/user-attachments/assets/3faa573b-67c1-4752-a30b-39ac58a18a1e)


第二步配置MTQQ
配置-->设备与服务-->添加集成  搜索 MTQQ 点击第一个开始配置，填写后打开高级进行配置，配置信息如下：

![image](https://github.com/user-attachments/assets/a0bb64be-9fb9-438d-a1a8-036cecd62b9c)


第三步开始测试通讯：如果你的8266上传了上面的代码，并连上网络。你通过遮挡红外传感器，这个地方就会收到信息。

![image](https://github.com/user-attachments/assets/05fd4d01-9ad3-4917-8a2a-eb937c408a30)


第四步配置homeassistant的configuration.yaml文件
复制代码文件到上面的configuration.yaml中即可，这个代码是deepseek自动生成，有一些内容虽然没用，但是缺了HA又不会工作，有大神可以帮修改优化。
第五步添加卡片，这个地方就不多说了。

![image](https://github.com/user-attachments/assets/1a2df1ab-fb20-4bd8-a92f-bc4b22326b2c)


### 7.传感器调试

首先将所需要的设备全部烧写完成就可以去表上调试，调试方法：把表首先固定在对应的位置，然后转动蓝色变阻器使得左边的LED不亮，然后停下来，观察当反光片过来的时候是否变亮，如果变亮证明正好，否则在这个附件调节。最终实现有反光片左侧的LED亮，没有反光片的时候不亮就调节好了。
![image](https://github.com/user-attachments/assets/cca88723-d226-49a7-96f1-29ef2c5259a3)

### 8.最终成果
![image](https://github.com/user-attachments/assets/9beae34a-0084-4711-b486-e2d1d1822a3d)

