# buptLab-datalink

这个仓库包含了北京邮电大学 2023-2024 春季学期《计算机网络》课程实验——数据链路层滑动窗口协议的设计与实现的相关代码和报告（见 Release）。

## 实现内容

设计的协议为滑动窗口协议，具体机制包含：

- 累计确认机制；
- 选择确认机制；
- 捎带确认机制；
- NAK 通知机制。

为优化信道利用率，还实现特性如下：

- CRC 纠错。

## 文件结构

```
.
├── crc32.c      # CRC32 校验和计算
├── crc_ec.cpp   # CRC 纠错实现
├── crc_ec.hpp   # CRC 纠错函数声明
├── datalink.c   # 数据链路层协议实现
├── datalink.h   # 协议参数规定
├── lprintf.c    # 日志输出
├── lprintf.h    # 日志函数声明
├── Makefile
├── protocol.c   # 信道模拟环境实现
├── protocol.h   # 信道相关函数声明
└── README.md
```
