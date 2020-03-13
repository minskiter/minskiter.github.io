---
title: poyla定理 
date: 2019-09-07 14:40:25
categories:
  - math
mathjax: true
---


**Poyla 定理**  
  设有n个对象，G是这n个对象上的置换群，用m种颜色涂染这n个对象，每个对象涂染一种颜色。若一种染色方案在群G的作用下变为另一种方案，则这两种方案当作是一种方案。那么存在的方案个数为：

$l=\frac{1}{G}[m^{c(a_1)}+m^{c(a_2)}+...+m^{c(a_n)}]=\frac{1}{G}[\sum_{i=1}^{n} m^{c(a_i)}]$

*其中G为对象的个数,$c(a_i)$为该置换的循环节数*

**例题**
[foj 1023][1]

[1]:http://acm.fzu.edu.cn/problem.php?pid=1023



