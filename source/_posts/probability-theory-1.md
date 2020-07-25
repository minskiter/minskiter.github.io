---
title: 基础概率论1
date: 2020-07-25 18:45:42
categories:
	- math
tags:
	- math
---



#### 概率

1. 统计概率
   $$
   P(A)=p\simeq f_n(A)=\frac mn
   $$

2. 古典概率
   $$
   P(A)=\frac{基数事件数}{所有可能数}=\frac mn
   $$

   - 组合（从n个取出m个）无放回排列
     $$
     A^m_n=\frac{n!}{n!-m!}
     $$

- 排列(从n个取出m个)无放回
  $$
  C_n^m=\frac{n!}{m!(n-m)!}=\frac{A_n^m}{m!}
  $$

3. 几何概率
   $$
   P(A)=\frac{m(A)}{m(\Omega)}=\frac{A的几何度量}{\Omega的几何度量}
   $$

#### 概率公式

1. 减法公式
   $$
   P(A-B)=P(A)-P(AB)
   $$

2. 加法公式
   $$
   P(A\cup B)=P(A)+P(B)-P(AB)
   $$

#### 条件概率

$$
P(A|B)=\frac{P(AB)}{P(B)}
$$

$$
P(AB)=P(A)P(B|A)=P(B)P(A|B)
$$

#### 全概率

$$
P(B)=\sum_{i=1}^{n}P(A_i)P(B|A_i)
$$

#### 贝叶斯公式

$$
P(A_i|B)=\frac{P(A_i)P(B|A_i)}{\sum_{k=1}^{n}P(A_k)P(B|A_k)}
$$

#### 独立性

$$
P(AB)=P(A)P(B)\Rightarrow AB独立
$$

$$
ABC相互独立，前三为两两相互独立\left\{
	\begin{array}{**lr**}
	P(AB)=P(A)P(B) & \\
	P(AC)=P(A)P(C) & \\
	P(BC)=P(B)P(C) & \\
	P(ABC)=P(A)P(B)P(C)
	\end{array}
\right.
$$

#### 等比数列

1. 定义式

$$
\frac{a_n}{a_{n-1}}=q (n>2,a_{n-1}\neq0,q\neq0)
$$

2. 通项公式：
   $$
   a_n=a_1\cdot q^{n-1}
   $$

3. 求和公式：
   $$
   S_n=na_1(q=1) \\
   S_n=\frac{a_1(1-q^n)}{1-q} (q\neq1)
   $$


#### 伯努利概率实验

1. 定义：每次实验中的概率不变，都是p，即P(A)=p

2. n重伯努利实验
   $$
   p_n(k)=C^k_np^k(1-p)^{n-k}(k=0,1,2,\cdots,n)
   $$

3. n重伯努利实验首次发送的概率
   $$
   g(k)=(1-p)^{k-1}p (k=1,2,\cdots)
   $$





