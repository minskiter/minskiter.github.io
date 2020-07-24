---
title: WEB通用权限管理设计
date: 2020-03-21 19:40:16
categories:
  - dotnet
tags:
  - dotnet
---

### 权限管理是WEB应用及其它应用所必须有的一个模块

#### 需求

- 不同管理员权限不同
- 用户所有者
- 不同用户可触及的资源不同
- 尽可能的高效

<!-- more -->

#### 构想

- 基于RESTFUL风格的WEB API，所有的资源都可以由Method+URL唯一的标识，因此权限可以由Method+URL作为微粒里划分
- 权限分配部分由管理员在UI界面分配
- 资源拥有者拥有稍微低于管理员的资源权限控制
- 基于角色权限分配,用户->角色->权限,并且为了提高查询效率，不让用户直接拥有权限

#### 难点

- 每次权限都需要执行sql，随着数据增加，权限不分表的情况下查询一次的时间过长，因此需要在内存中进行缓存
- 除了新增的资源，对于一些业务逻辑部分直接固定的URL如何自动添加到数据库里，而不是手动加入数据库
- 对于URL数据高效查询优化(在未来数据规模非常大的时候非常有必要)


#### 参考文档

[Microsoft Docs](https://docs.microsoft.com/en-us/aspnet/core/security/authorization/iauthorizationpolicyprovider?view=aspnetcore-3.1)




