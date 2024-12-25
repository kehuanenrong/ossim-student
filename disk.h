#ifndef DISK_H
#define DISK_H

#include <list>
#include <string>

// 磁盘磁头的运行方向：Inc--磁道号增加的方向；Dec--磁道号减小的方向。
enum Direction {
	Inc,
	Dec
};

class HardDisk {
    public:
        HardDisk();
        
        ~HardDisk();
        
        // 具有给定pid的进程请求使用磁盘读取/写入编号为 track_no 的磁道。
		// 按照磁盘调度算法将不同进程的IO请求插入该磁盘的IO队列中。 
		void Request(const int track_no, const int &pid);

        // 将当前使用磁盘的进程从磁盘移除并放回就绪队列。
		// 如果IO队列中还有其他进程在等待该磁盘，则按照磁盘调度算法选择一个进程使用该磁盘。
        int RemoveProcess();

        // 移除正在使用磁盘或在IO队列中的给定进程。
        void Remove(const int & pid);

        // 返回正使用磁盘的进程的PID。
        int GetCurrentProcess() const;
        
        // 返回当前进程正在读取或写入的当前磁道号。
        int GetCurrentTrack() const;

        // 设置当前使用磁盘的进程。
        void SetCurrentProcess(const int & pid);

        // 测试磁盘当前是否空闲。
        bool DiskIsIdle() const;

        // 输出当前磁盘的IO队列。
        void PrintQueue() const;

    private:
		Direction direction;									// 当前磁盘磁头的运行方向。
        int current_process;                                    // 当前使用磁盘的进程PID，如果为-1则表示磁盘空闲。
        int current_track;										// 当前正在读取和写入的磁道号。
        std::list<std::pair<int, int> > io_queue;        		// 磁盘的IO队列，其中每个元素为一个IO请求，包含了进程的PID和磁道号。
};

#endif // DISK_H
