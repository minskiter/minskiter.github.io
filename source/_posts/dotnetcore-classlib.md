---
title: dotnetcore 类库
date: 2020-07-24 12:03:09
categories:
	- dotnet
tags:
	- dotnet
---

### 环境

- vscode
- dotnet core 3.1 sdk
- centos 8

<!-- more -->

### 新建项目解决方案

``` sh
dotnet new sln
```

### 新建类库项目

``` sh
dotnet new classlib -o DockerLibrary
```

### 添加项目到解决方案

``` sh
dotnet sln add DockerLibrary/DockerLibrary.csproj
```

### 添加项目引用

``` sh
dotnet add ${MainProjectCsproj} reference ${LibraryCsproj}
```



