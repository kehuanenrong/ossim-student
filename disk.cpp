#include <string>
#include <sstream>
#include <iostream>

#include "disk.h"

using namespace std;

HardDisk::HardDisk() : direction(Inc), current_process(-1), current_track(0), io_queue(0) {}

HardDisk::~HardDisk() {}

// 具有给定pid的进程请求使用磁盘读取/写入编号为track_no的磁道。
// 按照磁盘调度算法将不同进程的IO请求插入该磁盘的IO队列中。 
void HardDisk::Request(const int track_no, const int &pid)
{
//    std::cout << "请编写程序并删除此行语句：具有给定pid的进程请求使用磁盘读取/写入编号为track_no的磁道。" << std::endl;

    // 如果磁盘空闲则直接分配，否则加入IO队列
    if (DiskIsIdle())
    {
        current_process = pid;
        current_track = track_no;
        direction = Inc; // 这里可根据实际需要决定磁头的移动方向
    }
    else
    {
//        io_queue.emplace_back(pid, track_no); // 用于存储磁盘的IO请求，记录进程的PID和对应的磁道号
        // SCAN插入：根据当前direction与track_no排序插入
        if (direction == Inc)
        {
            auto it = io_queue.begin();
            while (it != io_queue.end() && it->second <= track_no)
                it++;
            io_queue.insert(it, std::make_pair(pid, track_no));
        }
        else
        {
            auto it = io_queue.begin();
            while (it != io_queue.end() && it->second >= track_no)
                it++;
            io_queue.insert(it, std::make_pair(pid, track_no));
        }
    }
}

// 将当前使用磁盘的进程从磁盘移除并放回就绪队列。
// 如果IO队列中还有其他进程在等待该磁盘，则按照磁盘调度算法选择一个进程使用该磁盘。
int HardDisk::RemoveProcess()
{
//    std::cout << "请编写程序并删除此行语句：将当前使用磁盘的进程从磁盘移除并放回就绪队列。" << std::endl;

    int removed_process = current_process;
    current_process = -1;

    // 若 IO 队列不空，则从队首获取下一个进程使用磁盘
    if (!io_queue.empty())
    {
        auto next_request = io_queue.front();
        io_queue.pop_front();
        current_process = next_request.first;
        current_track = next_request.second;
        direction = Inc; // 可根据调度算法调整方向
    }

    return removed_process;
}

// 移除正在使用磁盘或在IO队列中的给定进程。
void HardDisk::Remove(const int &pid)
{
    // 若进程正在使用磁盘 
    if (current_process == pid)
    {
        RemoveProcess();
    }
    // 若进程在IO队列中 
    for (auto itr = io_queue.begin(); itr != io_queue.end(); itr++)
    {
        if ((*itr).first == pid)
        {
            io_queue.erase(itr);
        }
    }
}

// 返回正使用磁盘的进程的PID。
int HardDisk::GetCurrentProcess() const
{
    return current_process;
}

// 返回当前进程正在读取或写入的当前磁道号。 
int HardDisk::GetCurrentTrack() const
{
    return current_track;
}

// 设置当前使用磁盘的进程。
void HardDisk::SetCurrentProcess(const int &pid)
{
    current_process = pid;
}

// 测试磁盘当前是否空闲。
bool HardDisk::DiskIsIdle() const
{
    return current_process == -1;
}

// 输出当前磁盘的IO队列。
void HardDisk::PrintQueue() const
{
    for (auto itr = io_queue.begin(); itr != io_queue.end(); itr++)
    {
        std::cout << "<- [" << (*itr).first << " " << (*itr).second << "] ";
    }
    std::cout << std::endl;
}
