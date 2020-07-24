---
title: k8s kubeadm 集群部署实践
date: 2020-07-24 12:10:45
categories: 
	- kuberneters
tags:
	- kuberneters

---

### K8s Clusters Deploy

#### 环境

1. Centos 7
2. 内网 192.168.101.0/24

#### 工具

1. SSR（酸酸乳代理）
2. FRP

#### 集成一键脚本（脚本结构参考wait_for_it.sh)

https://github.com/minskiter/Centos7k8sScript

<!-- more -->

#### 实践步骤

##### 关闭swap分区

``` sh
sudo swapoff
sudo vi /etc/fstab
# swap 加#注释swap分区
```

##### 安装docker-ce

1. Remove Older Docker-ce

``` sh
sudo yum remove docker \
                  docker-client \
                  docker-client-latest \
                  docker-common \
                  docker-latest \
                  docker-latest-logrotate \
                  docker-logrotate \
                  docker-engine
```

2. Install Docker CE	

``` sh
sudo yum install -y yum-utils
sudo yum-config-manager \
    --add-repo \
    https://download.docker.com/linux/centos/docker-ce.repo
sudo yum install docker-ce docker-ce-cli containerd.io # Install Ce # Warning install containerd.io on Centos 8 first
```

3. Configuration

``` sh
sudo cat << CONFIGURE > /etc/docker/daemon.json
{
    "data-root": "/var/lib/docker",
    "registry-mirrors": [
        "https://docker.mirrors.ustc.edu.cn"
    ],
    "exec-opts": [
        "native.cgroupdriver=systemd"
    ]
}
CONFIGURE
vi /lib/systemd/system/docker.service
```

4. 配置Docker Remote API

``` sh
# 增加2375 Docker Remote API 
-H tcp://0.0.0.0:2375 
```

5. 开启防火墙

``` sh
sudo firewall-cmd --permanent --zone=public --add-port=2375/tcp
sudo firewall-cmd --reload
```

6. 增加HTTP代理

``` sh
sudo mkdir -p /etc/systemd/system/docker.service.d
cat << PROXY_SERVICE > /etc/systemd/system/docker.service.d/http-proxy.conf
[Service]
Environment="HTTP_PROXY=http://localhost:1080"
PROXY_SERVICE
```

*这里的http://localhost:1080 根据SSR修改*

7. 重启docker

 ``` sh
  systemctl daemon-reload
  systemctl restart docker
  docker info # 查看配置是否生效
 ```

##### 安装K8s

1. 添加Aliyun镜像

   ``` sh
   sudo cat <<EOF > /etc/yum.repos.d/kubernetes.repo
   [kubernetes]
   name=Kubernetes
   baseurl=http://mirrors.aliyun.com/kubernetes/yum/repos/kubernetes-el7-x86_64
   enabled=1
   gpgcheck=0
   repo_gpgcheck=0
   gpgkey=http://mirrors.aliyun.com/kubernetes/yum/doc/yum-key.gpg http://mirrors.aliyun.com/kubernetes/yum/doc/rpm-package-key.gpg
   EOF
   ```

2. 关闭selinux 

   ``` sh
   sudo setenforce 0
   ```

3. 安装k8s

   ``` sh
   sudo yum install -y kubelet kubeadm kubectl --disableexcludes=kubernetes
   sudo systemctl enable --now kubelet
   ```

##### 部署k8s

1. 修改hostname

   ``` sh
   hostnamectl set-hostname master
   ```

2. 修改hosts即本地DNS

   ``` sh
   sudo echo 127.0.0.1 $HOSTNAME >> /etc/hosts
   ```

3. 初始化集群

   ``` sh
   kubeadm init --pod-network-cidr=10.244.0.0/16
   ```

   *暂存join命令*token

4. 添加配置文件

   ``` sh
   mkdir -p $HOME/.kube
   sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
   sudo chown $(id -u):$(id -g) $HOME/.kube/config
   ```

5. 安装网络插件calico

   ``` sh
   mkdir -p $HOME/k8s/calico
   curl https://docs.projectcalico.org/manifests/tigera-operator.yaml -o $HOME/k8s/calico/tigera-operator.yaml
   cat << IMAGE_LIST > $HOME/k8s/calico/images.list.txt
   $(cat $HOME/k8s/calico/tigera-operator.yaml | grep image: | awk '{print $2}')
   IMAGE_LIST
   curl https://docs.projectcalico.org/manifests/custom-resources.yaml -o $HOME/k8s/calico/custom-resources.yaml
   cat << IMAGE_LIST2 >> $HOME/k8s/calico/images.list.txt
   $(cat $HOME/k8s/calico/custom-resources.yaml | grep image: | awk '{print $2}')
   IMAGE_LIST2
   sed -i "s|$(cat $HOME/k8s/calico/custom-resources.yaml | grep cidr | awk '{print $2}')|$CID|g" $HOME/k8s/calico/custom-resources.yaml
   docker pull $(cat $HOME/k8s/calico/images.list.txt)
   kubectl create -f $HOME/k8s/calico/tigera-operator.yaml
   kubectl create -f $HOME/k8s/calico/custom-resources.yaml
   ```

6. 开启防火墙

   ``` sh
   sudo firewall-cmd --permanent --zone=public --add-port=6443/tcp
   sudo firewall-cmd --permanent --zone=public --add-port=2379-2380/tcp
   sudo firewall-cmd --permanent --zone=public --add-port=10250-10252/tcp
   sudo firewall-cmd --permanent --zone=public --add-port=6443/udp
   sudo firewall-cmd --permanent --zone=public --add-port=2379-2380/udp
   sudo firewall-cmd --permanent --zone=public --add-port=10250-10252/udp
   sudo firewall-cmd --reload
   ```

7. 监视pod是否有正常启动

   ``` sh
   sudo watch kubectl get pods -A
   ```

PS: 安装完成后可以关闭docker代理

#### 添加节点

1. sh脚本将docker里的镜像导出
2. tar打包镜像
3. scp传送到子节点
4. 将tar文件解压并导入docker
5. 运行之前的kubeadm join命令

#### Trouble

1. 简单方法
   1. kubeadm reset # 还原重新再部署



