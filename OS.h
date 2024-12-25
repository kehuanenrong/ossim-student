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

        // ����һ���½��̲�������ӵ��������л� CPU������ö���Ϊ�գ���
		// �½��̵ĸ�����Ϊ����1 
        void CreateProcess();
        
        // ����һ���ӽ��̣��丸�����ǵ�ǰʹ��CPU���û����̣������˽���1֮��û���������̣��򱨴���
        void Fork();

        // ������������ӵ��������У�������� CPU ���У���ý���ֱ�ӽ���������С�
        void AddToReadyQueue(const int pid);

        // ��ʾ��ǰʹ�� CPU �Ľ��̣����г����������е����н��̡�
        void Snapshot() const;

        //����RR�����㷨����������к͵�ǰʹ��CPU�Ľ��̡�
        void CPUToReadyQueue();

        // ʹ��CPU�Ľ��̵���wait
        // �������û���ӽ��̣�������ȴ��� 
		// ��������Ѿ�ӵ������һ�������ӽ��̣���ɾ���ӽ��̲�����ʹ��CPU��
		// �������û�н����ӽ��̣���ý��̽���ȴ�״̬�������������ж��׵Ľ��̷��͸�CPU��
        void Wait();

		// ��ǰ����ʹ�� CPU �Ľ�����ֹ��
		// ����丸�������ڵȴ�����ý���������ֹ�����Ҹ����̽�ת���������е�ĩβ��
		// ���������û�еȴ����ý��̾ͻ��Ϊ��ʬ���̡� ����������ǽ��� 1����ý���������ֹ��
		// �ý��̵������ӽ��̶�����ֹ��
        void Exit();

        // ����������а����������̣���Ӿ���������ɾ���ý��̡� 
        void RemoveFromReadyQueue(const int pid_);

        // ���ÿ��Ӳ�����Ƿ��и������̣�����ý�������ʹ�ô��̲�λ������һ�� IO�����ϣ���Ὣ��ɾ����
        void RemoveFromDisks(const int & pid_);


        // ��ʾ�ڴ�״̬
		// ����ÿ��ʹ�õ�ҳ����ʾռ�����Ľ��̺��Լ����д洢��ҳ��š�
		// ҳ���ҳ��ı�Ŵ�0��ʼ
        void MemorySnapshot();

        // ��ǰ����ʹ��CPU�Ľ�������Ը����߼���ַ�����ڴ����
        void RequestMemoryOperation(const int & address);

        // ��ʾ��Щ���̵�ǰ����ʹ��Ӳ���Լ���Щ�������ڵȴ�ʹ������
        void IOSnapshot() const;
        
        // ��ǰʹ��CPU�Ľ����������disk_number��Ӳ�̵�track_no�Ŵŵ�
        // ��CPU���Ƴ���ǰ���̲��Ӿ���������ȡ���½���ִ��
		void RequestDisk(const int &disk_number, int track_no);

        // ��CPU���Ƴ���ǰ���̲��Ӿ���������ȡ���½���ִ�� 
        void GetNextFromReadyQueue();

        // �����һ������IO�󣬽�����̷Żص����������С�
        void RemoveProcessFromDisk(const int & disk_number);

        // �����������̵�ÿ��ҳ������ҵ��ý��̣�����ɾ����
        void RemoveFromFrames(const int & pid);

    private:
        int CPU;   								// ��ǰʹ��CPU�Ľ���PID
        int number_of_processes;    			// �Գ����������������Ľ�������
        int timestamp;							// �ڴ������ʱ���
        const int number_of_hard_disks;			// Ӳ������   	
        const unsigned int page_size;			// �ڴ�ҳ���С
        const unsigned int RAM;					// �ڴ�������
        const unsigned int number_of_frames;	// �ڴ��ҳ������
        std::list<int> ready_queue;				// �������У�����������̵�PID��
        std::vector<HardDisk*> hard_disks; 		// Ӳ��ָ���б�
        std::map<int, PCB*> all_processes;   	// �������н��̵�map���󣬴��н���PID�ͽ���PCB�ļ�ֵ�ԡ�
     
        //ҳ�� 
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

        std::vector<Frame*> frames;			// ����ҳ�� 

        // ɾ�����̵������ӽ��̣��������еĴ��̡�ҳ��IO���к;���������ɾ�����Ǽ���PCB��
        void DeleteChildren(PCB* pcb);
};

#endif //OS_H
