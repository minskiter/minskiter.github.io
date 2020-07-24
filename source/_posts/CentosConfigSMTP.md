---
title: 利用NetEase邮箱进行邮件验证 
date: 2019-09-07 14:40:25
categories:
    - linux
---


#### Aliyun 默认封25端口 所以只能转用465端口发邮件

<!-- more -->

1. 安装mailx（PS:使用前关闭默认的postfix）
    ``` bash
    yum install mailx
    ```

2. 创建一个用来存放 ssl 证书的目录
    ``` bash
    mkdir /{你的用户名}/.certs{可以修改为其它名字}
    ```

3. 我这里采用网易126邮箱
    ``` bash
    vim /etc/mail.rc # 默认的配置是在这个文件夹
    ```

4. 获取ssl 证书
    ``` bash
    echo -n | openssl s_client -connect smtp.126.com:465 | sed -ne '/-BEGIN CERTIFICATE-/,/-END CERTIFICATE-/p' > ~/.certs/126.crt # 向126请求证书
    ```   

5. 配置mail.rc
    ``` bash
    set from=xxx@126.com #之前设置好的邮箱地址
    set smtp=smtps://smtp.126.com:465 #邮件服务器
    set smtp-auth-user=xxx #用户名
    set smtp-auth-password=xxxx #授权码
    set smtp-auth=login #默认login即可
    set ssl-verify=ignore #ssl认证方式
    set nss-config-dir=/{你的用户名}/.certs #证书所在目录
    ```







