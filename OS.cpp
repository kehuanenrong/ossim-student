#include <string>
#include <sstream>
#include <iostream>

#include "OS.h"

using namespace std;

OperatingSystem::OperatingSystem(int &number_of_hard_disks_, unsigned int &RAM_, unsigned int &page_size_) : CPU(1),
                                                                                                             number_of_processes(1),
                                                                                                             timestamp(0),
                                                                                                             number_of_hard_disks(number_of_hard_disks_),
                                                                                                             page_size(page_size_),
                                                                                                             RAM(RAM_),
                                                                                                             number_of_frames(RAM_ / page_size_),
                                                                                                             ready_queue(0),
                                                                                                             hard_disks(number_of_hard_disks_),
                                                                                                             frames(0)
{
    // 创建初始进程 
    PCB *process_1 = new PCB{number_of_processes};
    process_1->SetParent(1); // 第一个进程（进程1）没有父进程 
    all_processes[number_of_processes] = process_1;

    // 初始化硬盘 
    for (int i = 0; i < number_of_hard_disks; i++)
    {
        HardDisk *disk_ = new HardDisk{};
        hard_disks[i] = disk_;
    }

    // 初始化页框
    for (unsigned int i = 0; i < number_of_frames; i++)
    {
        Frame *frame = new Frame{};
        frames.push_back(frame);
    }
}

OperatingSystem::~OperatingSystem()
{
    // 删除硬盘 
    for (auto disk : hard_disks)
    {
        delete disk;
    }
    // 删除页框 
    for (auto frame : frames)
    {
        delete frame;
    }
    // 删除所有PCB 
    for (auto PCB : all_processes)
    {
        delete PCB.second;
    }

    hard_disks.clear();
    frames.clear();
    all_processes.clear();
}

// 创建一个新进程并将其添加到就绪队列或 CPU（如果该队列为空）。
// 新进程的父进程为进程1 
void OperatingSystem::CreateProcess()
{
//    std::cout << "请编写程序并删除此行语句：创建一个新进程并将其添加到就绪队列或 CPU（如果该队列为空）。" << std::endl;

    number_of_processes++;
    PCB *new_process = new PCB{number_of_processes};
    new_process->SetParent(1); // 新进程的父进程为进程1
    all_processes[number_of_processes] = new_process;

    if (CPU == 1) // 如果 CPU 空闲
    {
        CPU = new_process->GetPid();
    }
    else
    {
        ready_queue.push_back(new_process->GetPid());
    }

}

// 创建一个子进程，其父进程是当前使用CPU的用户进程（若除了进程1之外没有其他进程，则报错）。
void OperatingSystem::Fork()
{
//    std::cout << "请编写程序并删除此行语句：建一个子进程，其父进程是当前使用CPU的用户进程（若除了进程1之外没有其他进程，则报错）。" << std::endl;

    if (CPU == 1)
    {
        std::cout << "错误：没有用户进程可以创建子进程。" << std::endl;
        return;
    }

    number_of_processes++;
    PCB *new_process = new PCB{number_of_processes};
    new_process->SetParent(CPU); // 新进程的父进程是当前使用CPU的用户进程
    all_processes[number_of_processes] = new_process;
    all_processes[CPU]->AddChildProcess(new_process);

    ready_queue.push_back(new_process->GetPid());

    cout << "已创建子进程：PID = " << number_of_processes << "，父进程 PID = " << CPU << endl;
}

// 将给定进程添加到就绪队列，或者如果 CPU 空闲，则该进程直接开始运行。
void OperatingSystem::AddToReadyQueue(const int pid)
{
//    std::cout << "请编写程序并删除此行语句：将给定进程添加到就绪队列，或者如果 CPU 空闲，则该进程直接开始运行。" << std::endl;

    if (CPU == 1) // 如果 CPU 空闲
    {
        CPU = pid;
    }
    else
    {
        ready_queue.push_back(pid);
    }

}

//按照RR调度算法处理就绪队列和当前使用CPU的进程。
void OperatingSystem::CPUToReadyQueue()
{
//    std::cout << "请编写程序并删除此行语句：按照RR调度算法(时间片轮转调度算法)处理就绪队列和当前使用CPU的进程。" << std::endl;

    if (CPU != 1) // 如果 CPU 不空闲
    {
        ready_queue.push_back(CPU); // 将当前使用CPU的进程放入就绪队列的末尾
    }
    GetNextFromReadyQueue(); // 从就绪队列中取出新进程执行
}

// 使用CPU的进程调用wait
// 如果进程没有子进程，则无需等待。 
// 如果进程已经拥有至少一个僵死子进程，则删除子进程并继续使用CPU。
// 如果进程没有僵死子进程，则该进程进入等待状态，并将就绪队列队首的进程发送给CPU。
void OperatingSystem::Wait()
{
    PCB *cpu_process = all_processes[CPU];

//    std::cout << "请编写程序并删除此行语句：占用CPU的进程调用wait。" << std::endl;

    if (!cpu_process->HasChildren())
    {
        return; // 如果进程没有子进程，则无需等待
    }

    for (auto child_pid : cpu_process->GetChildren())
    {
        PCB *child_process = all_processes[child_pid->GetPid()];
        if (child_process->IsZombieProcess())
        {
            // 如果进程已经拥有至少一个僵死子进程，则删除子进程并继续使用CPU
            DeleteChildren(child_process);
            delete child_process;
            all_processes.erase(child_pid->GetPid());
            return;
        }
    }

    // 如果进程没有僵死子进程，则该进程进入等待状态，并 将就绪队列队首的进程发送给CPU
    cpu_process->SetWaitingState(true);
    GetNextFromReadyQueue(); // 从就绪队列中取出新进程执行
}

// 当前正在使用CPU的进程终止
// 如果其父进程已在等待，则该进程立即终止，并且父进程将转到就绪队列的末尾。
// 如果父进程没有等待，该进程就会成为僵尸进程。 如果父进程是进程 1，则该进程立即终止。
// 该进程的所有子进程都被终止。
void OperatingSystem::Exit()
{
    PCB *exiting_process = all_processes[CPU];
    PCB *parent = all_processes[exiting_process->GetParent()];

//    std::cout << "请编写程序并删除此行语句：终止当前使用CPU的进程。" << std::endl;
//    cout << "正在删除进程: " << exiting_process->GetPid() << endl;

    // 如果就绪队列为空，则执行PID为1的初始进程 若此时CPU的PID为1则依旧不变
    if (ready_queue.empty() && (exiting_process->GetParent() == 1 || CPU == 1))
    {
        CPU = 1;
        return;
    }


    // 删除所有子进程
    DeleteChildren(exiting_process);

    // 如果父进程在等待
    if (parent->IsWaiting())
    {
        parent->SetWaitingState(false);
        AddToReadyQueue(parent->GetPid());
    }
    else if (parent->GetPid() != 1) // 如果父进程不是进程1
    {
        exiting_process->SetZombie(true);
    }


    // 删除当前进程
    all_processes.erase(exiting_process->GetPid());
    delete exiting_process;

    // 从就绪队列中取出新进程执行
    GetNextFromReadyQueue();


}

// 当前正在使用CPU的进程请求对给定逻辑地址进行内存操作
void OperatingSystem::RequestMemoryOperation(const int &address)
{
//    std::cout << "请编写程序并删除此行语句：当前正在使用CPU的进程请求对给定逻辑地址进行内存操作，实现分页的请求调入与LRU置换。" << std::endl;

    int page_number = address / page_size;
    int pid = CPU;

    // 查找空闲页框
    for (auto frame : frames)
    {
        if (frame->IsEmpty())
        {
            frame->pid_ = pid;
            frame->page_ = page_number;
            frame->timestamp_ = timestamp ++;
            return;
        }
    }

    // 如果没有空闲页框，使用LRU置换算法
    auto lru_frame = std::min_element(frames.begin(), frames.end(), [](Frame *a, Frame *b) {
        return a->timestamp_ < b->timestamp_;
    });

    (*lru_frame)->pid_ = pid;
    (*lru_frame)->page_ = page_number;
    (*lru_frame)->timestamp_ = timestamp ++;

//    timestamp++;

}

// 当前使用CPU的进程请求访问disk_number号硬盘的track_no号磁道
// 从CPU中移除当前进程并从就绪队列中取出新进程执行
void OperatingSystem::RequestDisk(const int &disk_number, int track_no)
{
    if ((disk_number < number_of_hard_disks) && (disk_number >= 0))
    {
        // 注意写代码判断：进程1作为系统进程在本系统中将不使用任何磁盘/添加到任何队列
//        std::cout << "请编写程序并删除此行语句：当前使用CPU的进程请求访问disk_number号硬盘的track_no号磁道。" << std::endl;

        if (CPU == 1)
        {
            std::cout << "错误：进程1作为系统进程在本系统中将不使用任何磁盘。" << std::endl;
            return;
        }

        PCB *current_process = all_processes[CPU];
        hard_disks[disk_number]->Request(track_no, current_process->GetPid());

        // 从CPU中移除当前进程并从就绪队列中取出新进程执行
        GetNextFromReadyQueue();
    }
    else
    {
        std::cout << "没有硬盘" << disk_number << std::endl;
    }
}

// 如果就绪队列包含给定进程，则从就绪队列中删除该进程。 
void OperatingSystem::RemoveFromReadyQueue(const int pid_)
{
    auto pid_to_delete = std::find(ready_queue.begin(), ready_queue.end(), pid_);
    // 如果在就绪队列中，则删除它。 
    if (pid_to_delete != ready_queue.end())
    {
        ready_queue.erase(pid_to_delete);
    }
}

// 检查每个硬盘上是否有给定进程，如果该进程正在使用磁盘并位于其中一个 IO队列上，则会将其删除。
void OperatingSystem::RemoveFromDisks(const int &pid_)
{
    for (auto itr = hard_disks.begin(); itr != hard_disks.end(); itr++)
    {
        (*itr)->Remove(pid_);
    }
}

// 显示当前使用 CPU 的进程，并列出就绪队列中的所有进程。
void OperatingSystem::Snapshot() const
{
    std::cout << "占用CPU的进程：" << CPU << std::endl;
    std::cout << "就绪队列";
    for (auto itr = ready_queue.begin(); itr != ready_queue.end(); ++itr)
    {
        std::cout << " <- " << *itr;
    }
    std::cout << std::endl;
}

// 显示内存状态
// 对于每个使用的页框，显示占用它的进程号以及其中存储的页面号。
// 页面和页框的编号从0开始
void OperatingSystem::MemorySnapshot()
{
    std::cout << "页框\t\t"
              << "页面\t\t"
              << "pid\t\t"
              << "时间戳" << std::endl;
    for (unsigned int i = 0; i < frames.size(); i++)
    {
//        std::cout << " " << i;
        if (frames[i]->pid_ != 0)
        {
            std::cout << " " << i << "\t\t" << frames[i]->page_ << "\t\t" << frames[i]->pid_ << "\t\t" << frames[i]->timestamp_;
        }
        std::cout << std::endl;
    }
}

// 显示哪些进程当前正在使用硬盘以及哪些进程正在等待使用它们
void OperatingSystem::IOSnapshot() const
{
    for (int i = 0; i < number_of_hard_disks; i++)
    {
        std::cout << "硬盘 " << i << ": ";
        if (hard_disks[i]->DiskIsIdle())
        {
            std::cout << "空闲" << std::endl;
        }
        else
        {
            std::cout << "[" << hard_disks[i]->GetCurrentProcess() << " " << hard_disks[i]->GetCurrentTrack() << "]" << std::endl;
            std::cout << "硬盘 " << i << "的IO队列: ";
            hard_disks[i]->PrintQueue();
        }
    }
}

// 从CPU中移除当前进程并从就绪队列中取出新进程执行 
void OperatingSystem::GetNextFromReadyQueue()
{
    if (ready_queue.empty())
    {
        CPU = 1;
    }
    else
    {
        CPU = ready_queue.front();
        ready_queue.pop_front();
    }
}

// 当完成一个磁盘IO后，将其进程放回到就绪队列中。
void OperatingSystem::RemoveProcessFromDisk(const int &disk_number)
{
    if ((disk_number < number_of_hard_disks) && (disk_number >= 0))
    {
        int removed_pcb = hard_disks[disk_number]->RemoveProcess();
        AddToReadyQueue(removed_pcb);
    }
    else
    {
        std::cout << "没有硬盘" << disk_number << std::endl;
    }
}

// 搜索给定进程的每个页框。如果找到该进程，则将其删除。
void OperatingSystem::RemoveFromFrames(const int &pid)
{
    for (auto itr = frames.begin(); itr != frames.end(); itr++)
    {
        if ((*itr)->pid_ == pid)
        {
            (*itr)->Clear();
        }
    }
}

// 删除进程的所有子进程，并从所有的磁盘、页框、IO队列和就绪队列中删除它们及其PCB。
void OperatingSystem::DeleteChildren(PCB *pcb)
{
    // 删除当前的进程的所有子进程 
    for (auto itr = pcb->GetChildren().begin(); itr != pcb->GetChildren().end(); itr++)
    {
        PCB *child = (*itr);
        if (child->HasChildren())
        {
            DeleteChildren(child);
        }
        else
        {
            RemoveFromDisks(child->GetPid());
            RemoveFromFrames(child->GetPid());
            RemoveFromReadyQueue(child->GetPid());

            // 从PCB表中移除子进程
            all_processes.erase(child->GetPid());
            delete child;
            child = nullptr;
        }
    }

    pcb->ClearChildren();

    // 保持当前进程的僵死状态并稍后删除PCB
    if (!pcb->IsZombieProcess())
    {
        all_processes.erase(pcb->GetPid());
    }

    RemoveFromDisks(pcb->GetPid());
    RemoveFromFrames(pcb->GetPid());
    RemoveFromReadyQueue(pcb->GetPid());
}
