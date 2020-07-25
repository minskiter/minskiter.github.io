---
title: 基础概率论2
date: 2020-07-25 18:46:45
categories:
	- math
tags:
	- math
---

#### 分布函数（证明是否是分布函数）

$$
F(X)=P(X\leq x),x\in(-\infty,+\infty)，记作F_X(x)非负有界性
$$

$$
F(-\infty)=\lim_{x\rightarrow-\infty}F(x)=0,F(+\infty)=\lim_{x\rightarrow+\infty}F(x)=1
$$



1. 单调不减性

$$
x_1<x_2,F(x_1)\leq F(x_2)
$$

2. 右连续性

$$
F(x+0)=F(x)
$$

$$
P(X>x)=1-F(x) \\
P(X<x)=F(x-0) \\
P(X=x)=F(x)-F(x-0) \\
P(x_1<X\leq x_2)=F(x_2)-F(x_1) \\
P(x_1\leq X \leq x_2)=F(x_2)-F(x_1 -0) \\
P(x_1<X<x_2)=F(x_2 -0)-F(x_1)
$$



### 离散型分布函数

1. 分布律（概率分布）

$$
P(X=x_k)=p_k,k=1,2,\cdots
$$

2. 分布列

   | X    | $x_1$ | $x_2$ | $\cdots$ | $x_k$ | $\cdots$ |
   | ---- | :---- | ----- | :------- | ----- | -------- |
   | P    | $p_1$ | $p_2$ | $\cdots$ | $p_k$ | $\cdots$ |

性质
$$
p_k\geq 0,k=1,2,\cdots \\
\sum_kp_k=1
$$

##### 01 分布(伯努利分布)

$$
P(X=1)=p,P(X=0)=1-p (0<p<1) \\
P(X=k)=p^k(1-p)^{1-k} (k=0,1;0<p<1) \\
X=X(\omega)=\left\{
\begin{array}{**rcl**}
0, 当\omega=\omega_1 & \\
1, 当\omega=\omega_2 \\
\end{array}
\right.
$$

##### 二项分布（n次伯努利实验）

$$
P(X=k)=C^k_np^kq^{n-k},k=0,1,2,\cdots,n;0<1=1-p<1 \\
X\sim B(n,p)
$$

二项分布的最有可能取值
$$
k_0=[(n+1)p]
$$

##### 泊松分布

$$
P(X=k)=e^{-\lambda}\frac{\lambda^k}{k!},k=0,1,2,\cdots \\
X\sim P(\lambda)
$$

$$
f(x)=e^x=\sum_{k=0}^\infty\frac{x^k}{k!}
$$

###### 泊松定理

$$
X^n\sim B(n,p_n)即P(X_n=k)=C^k_np^k_n(1-p_n)^{n-k},k=0,1,2,\cdots,n \\
当n很大，p很小的时候,np_n=\lambda>0 \\
\lim_{n\rightarrow\infty}P(X_n=k)=e^{-\lambda}\frac{\lambda^k}{k!},k=0,1,2,\cdots \\
$$

##### 几何分布

$$
P(X=k)=(1-p)^{k-1}p(k=1,2,\cdots) \\
X\sim G(p)
$$

##### 超几何分布（两类问题）

不重复抽取选择其中一类X的概率
$$
P(X=k)=\frac{C^k_MC^{n-k}_{N-M}}{C^n_N}(k=s,s+1,\cdots,t) \\
X\sim H(N,M,n)
$$

### 连续型随机变量

$$
F(x)=P(X\leq x)=\int_{-\infty}^xf(t)dt \\
概率密度f_X(x)
$$

1. $f(x)\geq0$
2. $\int_{-\infty}^{+\infty}f(x)dx=1$
3. $P(a<X\leq b)=F(b)-F(a)=\int_{a}^{b}f(x)dx$
4. $P(X=x)=F(x)-F(x-0)=0$
5. $F^{'}(x)=f(x)$

##### 均匀分布

$$
f(x)=\left\{
\begin{array}{**rlc**}
\frac{1}{b-a},& a<x<b \\
0, & 其它
\end{array}
\right. \\
X\sim U(a,b) \\
F(x)=\left\{
\begin{array}{**rlc**}
0, & x<a, \\
\frac{x-a}{b-a}, & a\leq x<b, \\
1, & x\geq b.
\end{array}
\right.
$$

##### 指数分布

$$
f(x)=\left\{
\begin{array}{**l**}
\lambda e^{-\lambda x} ,& x>0 \\
0, & x\leq 0
\end{array}
\right. \\
F(x)=\left\{
\begin{array}{**l**}
1-e^{-\lambda x}, & x>0, \\
0, & x\leq0.
\end{array}
\right.
$$

##### 正态分布

$$
f(x)=\frac{1}{\sigma\sqrt{2\pi}}e^{-\frac{(x-\mu)^2}{2\sigma^2}},-\infty<x<+\infty \\
F(x)=\frac{1}{\sigma\sqrt{2\pi}}\int_{-\infty}^{x}e^{-\frac{(t-\mu)^2}{2\sigma^2}}dt,-\infty<x<+\infty
$$

