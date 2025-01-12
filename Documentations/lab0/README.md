# Lab 0 鲲鹏开发板 openEuler 环境配置

官方镜像、烧录指南等，请参考 <http://www.orangepi.cn/html/hardWare/computerAndMicrocontrollers/service-and-support/Orange-Pi-kunpeng.html>

默认系统为 openEuler 22.03 LTS SP4，内核 5.10.10，烧录的镜像为 Kunpeng-Develop-openEuler-22.03-LTS-SP4-20241022-1438

flex 2.6.4、gcc 10.3.1、bison 3.8.2 已经装好了

毕昇编译器 4.0.0 B014，基于 LLVM 版本 17.0.6

要装 Autotuner，其依赖 Python 3.10，库里只有 3.9

安装 Python 3.10 并设为默认

```bash
wget https://www.python.org/ftp/python/3.10.15/Python-3.10.15.tgz
tar -zxvf Python-3.10.15.tgz
cd Python-3.10.15
./configure --enable-optimizations
make -j3
sudo make altinstall
sudo mv /usr/bin/python3 /usr/bin/python3.bak
sudo ln -s /usr/local/bin/python3.10 /usr/bin/python3
sudo mv /usr/bin/pip3 /usr/bin/pip3.bak
sudo ln -s /usr/local/bin/pip3.10 /usr/bin/pip3
```

安装 Autotuner

```bash
cd /opt/compiler/BiShengCompiler-4.0.0-aarch64-linux/bin
./install-autotuner.sh
```

验证安装

```bash
llvm-autotune -h
auto-tuner -h
```

如果遇到各种软件（如 `yum`/`dnf`）因替换 Python 而造成找不到包的问题，可以编辑其脚本，如 `/usr/bin/dnf`，将 `#!/usr/bin/python3` 改为 `#!/usr/bin/python3.9`。当然你也可以不替换全局 Python，仅替换 Autotuner 所需的 Python 版本，此处不详述，可以自行探索。

## 开发板不足的临时方案

华为并未发放足够数量的板子，所以可能不得不暂时跳过以上的配环境实验。现有一块板子位于助教处，上述环境已经配好，可供远程连接。

若要远程连接板子，请下载 [Tailscale](https://tailscale.com/)，将你的电脑添加至你的账户，然后向助教索取板子共享链接。将板子添加到你的账户后，同账户下的设备即可直接通过 Tailscale IP（100 开头，具体查看其网站，通常板子名称为 openEuler）来 SSH 到板子上，无须连接校园网，也可以通过 VS Code 等工具连接。但板子性能极其有限，不建议尝试开启图形界面。

为保证各位同学数据安全，默认每个人使用不同的账户，不添加至 sudoer。收到助教分配的账户后，建议尽快修改复杂的密码，助教不对认证造成的问题负责。若遇到系统卡住/连不上等问题，或合理的需要 root 权限的需求，请联系助教处理。
