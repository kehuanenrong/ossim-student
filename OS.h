#ifndef OS_H
#define OS_H

#include "PCB.h"
#include "disk.h"

#include <algorithm>
#include <vector>
#include <list>
#include <map>

class OperatingSystem {
    public:
        OperatingSystem(int & number_of_hard_disks_, unsigned int & RAM_, unsigned int & page_size_);

        ~OperatingSystem();

        // 创建一个新进程并将其添加到就绪队列或 CPU（如果该队列为空）。
		// 新进程的父进程为进程1 
        void CreateProcess();
        
        // 创建一个子进程，其父进程是当前使用CPU的用户进程（若除了进程1之外没有其他进程，则报错）。
        void Fork();

        // 将给定进程添加到就绪队列，或者如果 CPU 空闲，则该进程直接进入就绪队列。
        void AddToReadyQueue(const int pid);

        // 显示当前使用 CPU 的进程，并列出就绪队列中的所有进程。
        void Snapshot() const;

        //按照RR调度算法处理就绪队列和当前使用CPU的进程。
        void CPUToReadyQueue();

        // 使用CPU的进程调用wait
        // 如果进程没有子进程，则无需等待。 
		// 如果进程已经拥有至少一个僵死子进程，则删除子进程并继续使用CPU。
		// 如果进程没有僵死子进程，则该进程进入等待状态，并将就绪队列队首的进程发送给CPU。
        void Wait();

		// 当前正在使用 CPU 的进程终止。
		// 如果其父进程已在等待，则该进程立即终止，并且父进程将转到就绪队列的末尾。
		// 如果父进程没有等待，该进程就会成为僵尸进程。 如果父进程是进程 1，则该进程立即终止。
		// 该进程的所有子进程都被终止。
        void Exit();

        // 如果就绪队列包含给定进程，则从就绪队列中删除该进程。 
        void RemoveFromReadyQueue(const int pid_);

        // 检查每个硬盘上是否有给定进程，如果该进程正在使用磁盘并位于其中一个 IO队列上，则会将其删除。
        void RemoveFromDisks(const int & pid_);


        // 显示内存状态
		// 对于每个使用的页框，显示占用它的进程号以及其中存储的页面号。
		// 页面和页框的编号从0开始
        void MemorySnapshot();

        // 当前正在使用CPU的进程请求对给定逻辑地址进行内存操作
        void RequestMemoryOperation(const int & address);

        // 显示哪些进程当前正在使用硬盘以及哪些进程正在等待使用它们
        void IOSnapshot() const;
        
        // 当前使用CPU的进程请求访问disk_number号硬盘的track_no号磁道
        // 从CPU中移除当前进程并从就绪队列中取出新进程执行
		void RequestDisk(const int &disk_number, int track_no);

        // 从CPU中移除当前进程并从就绪队列中取出新进程执行 
        void GetNextFromReadyQueue();

        // 当完成一个磁盘IO后，将其进程放回到就绪队列中。
        void RemoveProcessFromDisk(const int & disk_number);

        // 搜索给定进程的每个页框。如果找到该进程，则将其删除。
        void RemoveFromFrames(const int & pid);

    private:
        int CPU;   								// 当前使用CPU的进程PID
        int number_of_processes;    			// 自程序启动以来创建的进程总数
        int timestamp;							// 内存请求的时间戳
        const int number_of_hard_disks;			// 硬盘数量   	
        const unsigned int page_size;			// 内存页面大小
        const unsigned int RAM;					// 内存总容量
        const unsigned int number_of_frames;	// 内存的页框总数
        std::list<int> ready_queue;				// 就绪队列，保存就绪进程的PID。
        std::vector<HardDisk*> hard_disks; 		// 硬盘指针列表
        std::map<int, PCB*> all_processes;   	// 保存所有进程的map对象，存有进程PID和进程PCB的键值对。
     
        //页框 
		struct Frame {
            int timestamp_;
            int page_;
            int pid_;

            Frame() : timestamp_(0), page_(0), pid_(0) {}
            ~Frame() {}

            bool IsEmpty() {
                return pid_ == 0;
            }
            
            void Clear() {
                timestamp_ = 0;
                page_ = 0;
                pid_ = 0;
            }
        };

        std::vector<Frame*> frames;			// 反置页表 

        // 删除进程的所有子进程，并从所有的磁盘、页框、IO队列和就绪队列中删除它们及其PCB。
        void DeleteChildren(PCB* pcb);
};

#endif //OS_H
