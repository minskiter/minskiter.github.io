---
layout: default
title: "校园网Win 10 RDP + 内网穿透"
date: 2019-12-25 14:40:25
categories: 随笔
---

#### 需求

- 由于校园网的 VPN 2MB 带宽过小，导致使用 windows10 RDP 连接时卡顿严重，体验极差。

#### 目标

- WIN10 RDP 流畅使用

#### 方法设想

- 考虑到实际在校园网电脑上使用时速度可以达到 6MB/s，且远程端带宽为 100M，理论上 RDP 1080P 流畅需要 5M 以上带宽即可。因此只要绕过校园网 VPN 这块短板即可达到流畅使用 RDP

#### 前提准备

- 100M 宽带动态公网 IP
- FRP 服务端和客户端
- 内网 ITX 台式电脑
- 外网树莓派 4B
- 阿里云备案 CN 域名

#### 步骤

1. 下载[FRP](https://github.com/fatedier/frp/releases)
2. 在树莓派上使用服务器端 frps

- 绑定高位端口（由于国内宽带容易查封低位端口，这里使用高位端口 50000+）
- 设置为 service，使其可以开启自启

3. DDNS

- 用于使用的是动态公网 IP，路由器每次重启都会导致 IP 发生变动，这里就需要动态域名解析
- 使用阿里云的 DNS API，将程序挂在树莓派以达到动态修改域名解析的作用,源码如下

```python
  #!/usr/bin/env python
  #coding=utf-8

  from aliyunsdkcore.client import AcsClient
  from aliyunsdkcore.acs_exception.exceptions import ClientException
  from aliyunsdkcore.acs_exception.exceptions import ServerException
  from aliyunsdkalidns.request.v20150109 import DescribeDomainRecordsRequest
  from aliyunsdkalidns.request.v20150109 import AddDomainRecordRequest
  from aliyunsdkalidns.request.v20150109 import DeleteDomainRecordRequest
  from aliyunsdkalidns.request.v20150109 import DescribeDomainRecordInfoRequest
  from aliyunsdkalidns.request.v20150109 import UpdateDomainRecordRequest
  import requests
  import re
  import json
  import random

  user_agent = [
      "Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10_6_8; en-us) AppleWebKit/534.50 (KHTML, like Gecko) Version/5.1 Safari/534.50",
      "Mozilla/5.0 (Windows; U; Windows NT 6.1; en-us) AppleWebKit/534.50 (KHTML, like Gecko) Version/5.1 Safari/534.50",
      "Mozilla/5.0 (Windows NT 10.0; WOW64; rv:38.0) Gecko/20100101 Firefox/38.0",
      "Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; .NET4.0C; .NET4.0E; .NET CLR 2.0.50727; .NET CLR 3.0.30729; .NET CLR 3.5.30729; InfoPath.3; rv:11.0) like Gecko",
      "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)",
      "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.0; Trident/4.0)",
      "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.0)",
      "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)",
      "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.6; rv:2.0.1) Gecko/20100101 Firefox/4.0.1",
      "Mozilla/5.0 (Windows NT 6.1; rv:2.0.1) Gecko/20100101 Firefox/4.0.1",
      "Opera/9.80 (Macintosh; Intel Mac OS X 10.6.8; U; en) Presto/2.8.131 Version/11.11",
      "Opera/9.80 (Windows NT 6.1; U; en) Presto/2.8.131 Version/11.11",
      "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_7_0) AppleWebKit/535.11 (KHTML, like Gecko) Chrome/17.0.963.56 Safari/535.11",
      "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; Maxthon 2.0)",
      "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; TencentTraveler 4.0)",
      "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)",
      "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; The World)",
      "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; Trident/4.0; SE 2.X MetaSr 1.0; SE 2.X MetaSr 1.0; .NET CLR 2.0.50727; SE 2.X MetaSr 1.0)",
      "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; 360SE)",
      "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; Avant Browser)",
      "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)",
      "Mozilla/5.0 (iPhone; U; CPU iPhone OS 4_3_3 like Mac OS X; en-us) AppleWebKit/533.17.9 (KHTML, like Gecko) Version/5.0.2 Mobile/8J2 Safari/6533.18.5",
      "Mozilla/5.0 (iPod; U; CPU iPhone OS 4_3_3 like Mac OS X; en-us) AppleWebKit/533.17.9 (KHTML, like Gecko) Version/5.0.2 Mobile/8J2 Safari/6533.18.5",
      "Mozilla/5.0 (iPad; U; CPU OS 4_3_3 like Mac OS X; en-us) AppleWebKit/533.17.9 (KHTML, like Gecko) Version/5.0.2 Mobile/8J2 Safari/6533.18.5",
      "Mozilla/5.0 (Linux; U; Android 2.3.7; en-us; Nexus One Build/FRF91) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Mobile Safari/533.1",
      "MQQBrowser/26 Mozilla/5.0 (Linux; U; Android 2.3.7; zh-cn; MB200 Build/GRJ22; CyanogenMod-7) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Mobile Safari/533.1",
      "Opera/9.80 (Android 2.3.4; Linux; Opera Mobi/build-1107180945; U; en-GB) Presto/2.8.149 Version/11.10",
      "Mozilla/5.0 (Linux; U; Android 3.0; en-us; Xoom Build/HRI39) AppleWebKit/534.13 (KHTML, like Gecko) Version/4.0 Safari/534.13",
      "Mozilla/5.0 (BlackBerry; U; BlackBerry 9800; en) AppleWebKit/534.1+ (KHTML, like Gecko) Version/6.0.0.337 Mobile Safari/534.1+",
      "Mozilla/5.0 (hp-tablet; Linux; hpwOS/3.0.0; U; en-US) AppleWebKit/534.6 (KHTML, like Gecko) wOSBrowser/233.70 Safari/534.6 TouchPad/1.0",
      "Mozilla/5.0 (SymbianOS/9.4; Series60/5.0 NokiaN97-1/20.0.019; Profile/MIDP-2.1 Configuration/CLDC-1.1) AppleWebKit/525 (KHTML, like Gecko) BrowserNG/7.1.18124",
      "Mozilla/5.0 (compatible; MSIE 9.0; Windows Phone OS 7.5; Trident/5.0; IEMobile/9.0; HTC; Titan)",
      "UCWEB7.0.2.37/28/999",
      "NOKIA5700/ UCWEB7.0.2.37/28/999",
      "Openwave/ UCWEB7.0.2.37/28/999",
      "Mozilla/4.0 (compatible; MSIE 6.0; ) Opera/UCWEB7.0.2.37/28/999",
      # iPhone 6：
    "Mozilla/6.0 (iPhone; CPU iPhone OS 8_0 like Mac OS X) AppleWebKit/536.26 (KHTML, like Gecko) Version/8.0 Mobile/10A5376e Safari/8536.25",
  ]

  header={
    'User-Agent':random.choice(user_agent)
  }

  def getIp():
    html_text = requests.get("https://www.ipip.net/ip.html",headers=header).text
    search = re.search(r'name="ip" value="(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})"',html_text)
    return search.group(1)

  # 新建客户端
  client = AcsClient('id', 'token', 'cn-hangzhou')

  domain = '{your domain}'

  def AddDns(Domain,Type,RR,Value):
    print('添加DNS记录[域名]:'+Domain+' [类型]:'+Type+' [记录值]:'+RR+' [值]:'+Value)
    request = AddDomainRecordRequest.AddDomainRecordRequest()
    request.set_accept_format('json')
    request.set_DomainName(Domain)
    request.set_RR(RR)
    request.set_Type(Type)
    request.set_Value(Value)
    response = client.do_action(request)
    return json.loads(str(response, encoding='utf-8'))

  def GetDnsList(Domain):
    request = DescribeDomainRecordsRequest.DescribeDomainRecordsRequest()
    request.set_DomainName(Domain)
    request.set_accept_format('json')
    response = client.do_action(request)
    return json.loads(str(response, encoding='utf-8'))

  def UpdateRecord(RecordId,Type,RR,Value):
    print('更新DNS记录 '+'[类型]:'+Type+' [记录值]:'+RR+' [值]:'+Value)
    request = UpdateDomainRecordRequest.UpdateDomainRecordRequest()
    request.set_accept_format('json')
    request.set_RecordId(RecordId)
    request.set_Type(Type),
    request.set_RR(RR),
    request.set_Value(Value)
    response = client.do_action(request)
    return json.loads(str(response, encoding='utf-8'))

  def GetRecord(Domain,Type,RR):
    print('获取DNS记录[域名]:'+Domain+' [类型]:'+Type+' [记录值]:'+RR)
    for item in GetDnsList(domain)['DomainRecords']['Record']:
      if (item['RR']==RR and item['Type']==Type and item['DomainName']==Domain):
        return item

  def DelRecord(RecordId):
    request = DeleteDomainRecordRequest.DeleteDomainRecordRequest()
    request.set_RecordId(RecordId)
    res = client.do_action(request)
    return json.loads(str(res, encoding='utf-8'))


  ip = getIp()

  try:
    Record = GetRecord(domain,'A','frp')
    if Record['Value']!=ip:
      RecordId = Record['RecordId']
      res= UpdateRecord(RecordId,'A','frp',ip)
      if 'Message' in res:
        print(res['Message'])
      else:
        print('更新成功')
      # DelRecord(RecordId)
      # AddDns(domain,'A','frp',ip)
      # if 'Message' in res:
      #   print(res['Message'])
      # else:
      #   print('更新成功')
    else:
      print('当前的ip没有变化:' + ip)
  except:
    res = AddDns(domain,'A','frp',ip)
    if 'Message' in res:
      print(res['Message'])
    else:
      print('添加成功')
```

4. 内网电脑开启远程连接

- 电脑-属性-远程设置-允许远程连接
- 下载 nssm 服务包装工具（由于直接使用 win10 的 sc.exe 创建服务批处理会报 1503 错误）
- 写入批处理代码运行 frpc 客户端(相关配置详见 frpc_full.ini)

``` bash
powershell C:\frpc -c C:\frpc.ini
```

- nssm 将批处理包装为服务，设置为 start=auto

5. END

#### 最终效果

- 随时随地流畅连接内网电脑

#### 注意要点

- 内网电脑睡眠需关闭，主板设置开启自启以及定时自启以此来防止突然断电的情况
- 注意防火墙开启对应端口
