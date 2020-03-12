---
title: Python 定向爬虫python 
date: 2019-09-07 14:40:25
categories:
    - python
---


#### 爬虫目标

1. 爬取[豆瓣电影](https://movie.douban.com/)的电影详情、剧情介绍
2. 根据豆瓣所爬取的电影名称到[电影天堂](https://www.dytt8.net/) 寻找相应电影的下载链接ftp和磁性链接magnet
3. 将所得数据存放于数据库中或文件中

#### 爬虫功能

1. URLMangage 管理可爬取URL链接，存放于MYSQL数据库中，用以判重，断点续爬
2. URLcode URL解析，解析protocol协议，domain域名，path相对路径，以及判断url所对应的文件类型或目录
3. Spider 爬虫主程序，通过给定的URL入口进行递归爬取

#### 依赖第三方库

1. requests HTTP请求
2. BeautifulSoup HTML地方库
3. lxml xml解析
4. pymysql mysql的py库

#### urlmanage类，用来管理要爬取的url的存储
``` python
#!/usr/bin/python3
# -*- coding: UTF-8 -*-

import pymysql
import hashlib
import base64
import json


class UrlManage:
    """ URL数据存储

    存储url链接数据
    从url-config.json中加载数据库信息
    举个栗子:
        url-config.json(code:utf-8)像这样:
            {"host":"localhost","user":"root","password":"exam","database","spider"}

    方法:
        create(),geturl(),exists(),insert(),update()

    """

    def __init__(self):
        """ 构造函数
            从url-config中加载信息 
        """
        def loadconfig():
            with open('url-config.json', 'r', encoding='utf-8') as config_file:
                return json.load(config_file)
        try:
            config = loadconfig()
        except:
            print('请检查config配置')
            return
        self.__db = pymysql.connect(
            config['host'], config['user'], config['password'], config['database'])

    def __del__(self):
        """ 析构函数
        """
        self.__db.close()

    def reset(self):
        """ 重新初始化数据表 """

        cursor = self.__db.cursor()
        try:
            cursor.execute(r"""
                drop table urls;
            """)
            self.create()
        except:
            print('未知错误')


    def create(self):
        """ 创建初始数据表
            如果不存在才创建，已存在将不会重复创建
        """
        cursor = self.__db.cursor()
        try:
            cursor.execute(r"""
                create table if not exists `urls`(
                `id` varchar(255) not null comment 'url的md5值',
                `url` varchar(255) not null comment 'url链接base64编码',
                `update` datetime default now() comment '更新时间',
                `status` int(4) default 0 comment '链接状态 0 未搜索 1 已搜索 2 超时 3 死链接',
                primary key (`id`)
                ) comment='爬虫URL仓库';
            """)
        except:
            print('数据表创建未知错误')

    def geturl(self, status):
        """获取一条url状态为status的值

            Args:
                status: 链接状态 0 已搜索 1 未搜索 2 超时 3 失败
        """
        if status < 0 and status > 3:
            return None
        cursor = self.__db.cursor()
        cursor.execute(""
                       "select url from urls where status=" +
                       str(status)+" limit 1;"  # 匹配所有状态未0即未搜索的网址
                       "")
        try:
            for i in cursor.fetchone():
                return base64.decodebytes(bytes(i, 'utf-8')).decode('utf-8')
        except:
            return None

    def exists(self, url):
        """判断链接是否存在数据库里

            Args:
                url: url链接

            Returns:
                bool: True or False 真代表存在，假代表不存在
        """
        cursor = self.__db.cursor()
        md5 = hashlib.md5(url.encode('utf-8')).hexdigest()

        cursor.execute("select url from urls where id='"+md5+"';")
        if cursor.fetchone():
            return True
        else:
            return False

    def insert(self, url):
        """插入新的未搜索的链接

            Args:
                url: 链接
        """
        cursor = self.__db.cursor()
        md5 = hashlib.md5(url.encode('utf-8')).hexdigest()
        baseUrl = base64.encodebytes(bytes(url, 'utf-8'))
        baseUrl = baseUrl.decode('utf-8')
        try:
            #print("\r{} 插入成功".format(url),end="")
            cursor.execute("insert into urls (id,url) values('" +
                           md5+"','"+baseUrl+"');")  # SQL 插入网址
            self.__db.commit()
            
        except:
            self.__db.rollback()

    def update(self, url, status):
        """更新url状态

            Args:
                url: url链接
                status: 状态 0 未搜索 1 已搜索 2 超时 3失败
        """
        cursor = self.__db.cursor()
        md5 = hashlib.md5(url.encode('utf-8')).hexdigest()
        try:
            cursor.execute("update urls set status="+str(status) +
                           ", `update`=now() where id='"+md5+"'")  # 更新状态
            self.__db.commit()
        except:
            self.__db.rollback()


def main():
    """测试用主函数
    """
    pass


if __name__ == "__main__":
    main()

```

#### urlcode类 用来解析url链接

``` python
#!/usr/bin/python3
# -*- coding: UTF-8 -*-
import re


class UrlCode:
    """ url解析 

        主要方法:
            decode(): 
    """

    def __init__(self):
        self.filetype = [
            'mkv', 'html', 'rmvb', 'mp3',
            'mp4', 'jpg', 'php', 'txt',
            'md'
        ]

    def decode(self, url: str):
        """解码：对url进行解码，分解出protocol、domain、path

            Args:
                url(string): url链接

            Returns:
                url的字典类型包括protocol、domain、path
                例如磁力链接等等，域名可能为空
                解码失败返回None
        """
        box = {}  # 创建空的dict用以存取属性
        try:
            match = re.match(r'(https?|ftp|ssr|magnet|thunder){1}', url)
            if match:
                box['protocol'] = match.group()
                match = re.search(r'[\w\.]+', url[match.end():])
            else:
                box['protocol'] =""
                match = None
            if match:
                box['domain'] = match.group()
                box['path'] = url[len(box['protocol'])+match.end():]
            else:
                box['domain'] = ''
                box['path'] = url[len(box['protocol']):]

            if len(box['path']) <= 0:
                box['path'] = '/'
        except:
            print('链接:{} 解码失败'.format(url))
            return None
        return box

    def encode(self, **args):
        """对dict类型的url或者protocol,domain,path进行编码

        Args:
            url: 字典类型，必须包含domain、protocol、path属性
            或者
            protocol: 协议
            domain: 域名
            path: 路径

        Returns:
            url字符串，若编码失败则返回None
        """
        url = None
        try:
            if 'url' in args:
                url = args['url']['protocol']+'://' + \
                    args['url']['domain']+args['url']['path']
            elif 'domain' in args:
                url = args['protocol']+'://'+args['domain']+args['path']
            else:
                print('参数错误')
        except:
            print('参数错误')
        return url

    def types(self, url: str):
        """判断url链接文件的类型

            Args:
                url:url链接

            Returns:
                urlType字符串，默认返回path，异常返回None
        """
        try:
            url_type = 'path'  # 默认为路径
            for urltype in self.filetype:
                pat = '('+urltype+')$'
                match = re.search(pat, url)
                if match:
                    url_type = match.group()
            return url_type
        except:
            return None


def main():
    """ 测试用主函数"""


if __name__ == "__main__":
    main()

```

#### 爬虫主代码，暂时只实现了爬取豆瓣的json

``` python
#!/usr/bin/python3
# -*- coding: UTF-8 -*-

import requests
import lxml
import re
import json
import time
from bs4 import BeautifulSoup
from urlManage import UrlManage
from urlCode import UrlCode


def save_data(filename, data):
    """ 保存数据到文件
    """
    with open(filename, "a", encoding='utf-8') as file:
        file.write(data)


class lolSpider:
    """ 爬虫封装类，lol就好了 """

    def __init__(self, domain: str, protocol='https', filters='', interval=0.0):
        """ 初始化函数

            Args:
                domain: 限制在该域名下搜索
                protocol: 网站的协议
                filters: 获取跟filter匹配的url，可用正则表达式

        """
        self.status = 0
        self.count = 0
        self.time = time.time()
        self.domain = domain
        self.protocol = protocol
        self.filters = filters
        self.html = None
        self.interval = interval  # 间隔最小的时间
        self.headers = {
            'user-agent': 'Mozilla/5.0 (Windows NT 10.0; '
            'Win64; x64) AppleWebKit/537.36'
            ' (KHTML, like Gecko) Chrome/74.'
            '0.3729.169 Safari/537.36'
        }  # 定义头文件

    def get(self, url):
        """ 获取url对应的html的信息

            Args:
                url: url链接
        """
        try:
            reponse = requests.get(
                url,
                headers=self.headers,
                timeout=10
            )
            reponse.raise_for_status()
            reponse.encoding = "utf-8"
            self.html = reponse.text
        except requests.Timeout:
            print("\r{} 链接爬取超时".format(url), end="")
            self.html = None
            self.status = 2
        except:
            print('\r{} 链接爬取失败'.format(url), end="")
            self.html = None
            self.status = 3
        self.status = 1

    def geturl(self, urlmanage):
        if not self.html:
            self.soup = None
            return None
        urlparser = UrlCode()
        self.soup = BeautifulSoup(self.html, 'lxml')
        a = self.soup.findAll('a')
        https = re.compile(r'^https?')
        for item in a:
            try:
                url = item.attrs['href']
                # url 转为dict
                url = urlparser.decode(url)
                if url['protocol'] == '' and url['path'][:1] == '/':
                    url = urlparser.encode(
                        protocol=self.protocol, domain=self.domain, path=url['path'])
                    urlmanage.insert(url)
                elif https.match(url['protocol']):
                    types = urlparser.types(url['path'])
                    if bool(types) and types == 'path' and url['domain'] == self.domain:
                        url = urlparser.encode(url=url)
                        urlmanage.insert(url)
            except:
                continue

    def downloader(self):
        if not self.html or not self.soup:
            return
        js = self.soup.findAll('script')
        for item in js:
            try:
                if item.attrs['type'] == "application/ld+json":
                    save_data('movie.txt', item.string)
            except:
                continue

        pass

    def crawl(self, urlmanage):
        self.time = time.time()
        try:
            self.count += 1
            url = urlmanage.geturl(0)
            self.get(url)
            urlmanage.update(url, self.status)
            self.geturl(urlmanage)
            self.downloader()
            intervaltime = round(time.time()-self.time, 3)
            print("当前爬取{}个网站，当前速度{}s/个".format(self.count,
                                               max(self.interval, intervaltime)))
            # 遵守robots.txt 5s间隔
            time.sleep(max(0.0, self.interval-intervaltime))
        except TypeError:
            print('\rcrawl 参数错误', end="")
            return


# TEST: 测试用主代码
if __name__ == "__main__":
    URLS = [
        'https://movie.douban.com/'
    ]
    DOMAIN = 'movie.douban.com'

    urlmanage = UrlManage()
    # urlmanage.reset()
    spider = lolSpider(domain=DOMAIN, interval=5.0)
    for item in URLS:
        urlmanage.insert(item)
    while True:
        spider.crawl(urlmanage)

```
