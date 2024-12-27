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
    // ������ʼ���� 
    PCB *process_1 = new PCB{number_of_processes};
    process_1->SetParent(1); // ��һ�����̣�����1��û�и����� 
    all_processes[number_of_processes] = process_1;

    // ��ʼ��Ӳ�� 
    for (int i = 0; i < number_of_hard_disks; i++)
    {
        HardDisk *disk_ = new HardDisk{};
        hard_disks[i] = disk_;
    }

    // ��ʼ��ҳ��
    for (unsigned int i = 0; i < number_of_frames; i++)
    {
        Frame *frame = new Frame{};
        frames.push_back(frame);
    }
}

OperatingSystem::~OperatingSystem()
{
    // ɾ��Ӳ�� 
    for (auto disk : hard_disks)
    {
        delete disk;
    }
    // ɾ��ҳ�� 
    for (auto frame : frames)
    {
        delete frame;
    }
    // ɾ������PCB 
    for (auto PCB : all_processes)
    {
        delete PCB.second;
    }

    hard_disks.clear();
    frames.clear();
    all_processes.clear();
}

// ����һ���½��̲�������ӵ��������л� CPU������ö���Ϊ�գ���
// �½��̵ĸ�����Ϊ����1 
void OperatingSystem::CreateProcess()
{
//    std::cout << "���д����ɾ��������䣺����һ���½��̲�������ӵ��������л� CPU������ö���Ϊ�գ���" << std::endl;

    number_of_processes++;
    PCB *new_process = new PCB{number_of_processes};
    new_process->SetParent(1); // �½��̵ĸ�����Ϊ����1
    all_processes[number_of_processes] = new_process;

    if (CPU == 1) // ��� CPU ����
    {
        CPU = new_process->GetPid();
    }
    else
    {
        ready_queue.push_back(new_process->GetPid());
    }

}

// ����һ���ӽ��̣��丸�����ǵ�ǰʹ��CPU���û����̣������˽���1֮��û���������̣��򱨴���
void OperatingSystem::Fork()
{
//    std::cout << "���д����ɾ��������䣺��һ���ӽ��̣��丸�����ǵ�ǰʹ��CPU���û����̣������˽���1֮��û���������̣��򱨴���" << std::endl;

    if (CPU == 1)
    {
        std::cout << "����û���û����̿��Դ����ӽ��̡�" << std::endl;
        return;
    }

    number_of_processes++;
    PCB *new_process = new PCB{number_of_processes};
    new_process->SetParent(CPU); // �½��̵ĸ������ǵ�ǰʹ��CPU���û�����
    all_processes[number_of_processes] = new_process;
    all_processes[CPU]->AddChildProcess(new_process);

    ready_queue.push_back(new_process->GetPid());

    cout << "�Ѵ����ӽ��̣�PID = " << number_of_processes << "�������� PID = " << CPU << endl;
}

// ������������ӵ��������У�������� CPU ���У���ý���ֱ�ӿ�ʼ���С�
void OperatingSystem::AddToReadyQueue(const int pid)
{
//    std::cout << "���д����ɾ��������䣺������������ӵ��������У�������� CPU ���У���ý���ֱ�ӿ�ʼ���С�" << std::endl;

    if (CPU == 1) // ��� CPU ����
    {
        CPU = pid;
    }
    else
    {
        ready_queue.push_back(pid);
    }

}

//����RR�����㷨����������к͵�ǰʹ��CPU�Ľ��̡�
void OperatingSystem::CPUToReadyQueue()
{
//    std::cout << "���д����ɾ��������䣺����RR�����㷨(ʱ��Ƭ��ת�����㷨)����������к͵�ǰʹ��CPU�Ľ��̡�" << std::endl;

    if (CPU != 1) // ��� CPU ������
    {
        ready_queue.push_back(CPU); // ����ǰʹ��CPU�Ľ��̷���������е�ĩβ
    }
    GetNextFromReadyQueue(); // �Ӿ���������ȡ���½���ִ��
}

// ʹ��CPU�Ľ��̵���wait
// �������û���ӽ��̣�������ȴ��� 
// ��������Ѿ�ӵ������һ�������ӽ��̣���ɾ���ӽ��̲�����ʹ��CPU��
// �������û�н����ӽ��̣���ý��̽���ȴ�״̬�������������ж��׵Ľ��̷��͸�CPU��
void OperatingSystem::Wait()
{
    PCB *cpu_process = all_processes[CPU];

//    std::cout << "���д����ɾ��������䣺ռ��CPU�Ľ��̵���wait��" << std::endl;

    if (!cpu_process->HasChildren())
    {
        return; // �������û���ӽ��̣�������ȴ�
    }

    for (auto child_pid : cpu_process->GetChildren())
    {
        PCB *child_process = all_processes[child_pid->GetPid()];
        if (child_process->IsZombieProcess())
        {
            // ��������Ѿ�ӵ������һ�������ӽ��̣���ɾ���ӽ��̲�����ʹ��CPU
            DeleteChildren(child_process);
            delete child_process;
            all_processes.erase(child_pid->GetPid());
            return;
        }
    }

    // �������û�н����ӽ��̣���ý��̽���ȴ�״̬���� ���������ж��׵Ľ��̷��͸�CPU
    cpu_process->SetWaitingState(true);
    GetNextFromReadyQueue(); // �Ӿ���������ȡ���½���ִ��
}

// ��ǰ����ʹ��CPU�Ľ�����ֹ
// ����丸�������ڵȴ�����ý���������ֹ�����Ҹ����̽�ת���������е�ĩβ��
// ���������û�еȴ����ý��̾ͻ��Ϊ��ʬ���̡� ����������ǽ��� 1����ý���������ֹ��
// �ý��̵������ӽ��̶�����ֹ��
void OperatingSystem::Exit()
{
    PCB *exiting_process = all_processes[CPU];
    PCB *parent = all_processes[exiting_process->GetParent()];

//    std::cout << "���д����ɾ��������䣺��ֹ��ǰʹ��CPU�Ľ��̡�" << std::endl;
//    cout << "����ɾ������: " << exiting_process->GetPid() << endl;

    // �����������Ϊ�գ���ִ��PIDΪ1�ĳ�ʼ���� ����ʱCPU��PIDΪ1�����ɲ���
    if (ready_queue.empty() && (exiting_process->GetParent() == 1 || CPU == 1))
    {
        CPU = 1;
        return;
    }


    // ɾ�������ӽ���
    DeleteChildren(exiting_process);

    // ����������ڵȴ�
    if (parent->IsWaiting())
    {
        parent->SetWaitingState(false);
        AddToReadyQueue(parent->GetPid());
    }
    else if (parent->GetPid() != 1) // ��������̲��ǽ���1
    {
        exiting_process->SetZombie(true);
    }


    // ɾ����ǰ����
    all_processes.erase(exiting_process->GetPid());
    delete exiting_process;

    // �Ӿ���������ȡ���½���ִ��
    GetNextFromReadyQueue();


}

// ��ǰ����ʹ��CPU�Ľ�������Ը����߼���ַ�����ڴ����
void OperatingSystem::RequestMemoryOperation(const int &address)
{
//    std::cout << "���д����ɾ��������䣺��ǰ����ʹ��CPU�Ľ�������Ը����߼���ַ�����ڴ������ʵ�ַ�ҳ�����������LRU�û���" << std::endl;

    int page_number = address / page_size;
    int pid = CPU;

    // ���ҿ���ҳ��
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

    // ���û�п���ҳ��ʹ��LRU�û��㷨
    auto lru_frame = std::min_element(frames.begin(), frames.end(), [](Frame *a, Frame *b) {
        return a->timestamp_ < b->timestamp_;
    });

    (*lru_frame)->pid_ = pid;
    (*lru_frame)->page_ = page_number;
    (*lru_frame)->timestamp_ = timestamp ++;

//    timestamp++;

}

// ��ǰʹ��CPU�Ľ����������disk_number��Ӳ�̵�track_no�Ŵŵ�
// ��CPU���Ƴ���ǰ���̲��Ӿ���������ȡ���½���ִ��
void OperatingSystem::RequestDisk(const int &disk_number, int track_no)
{
    if ((disk_number < number_of_hard_disks) && (disk_number >= 0))
    {
        // ע��д�����жϣ�����1��Ϊϵͳ�����ڱ�ϵͳ�н���ʹ���κδ���/��ӵ��κζ���
//        std::cout << "���д����ɾ��������䣺��ǰʹ��CPU�Ľ����������disk_number��Ӳ�̵�track_no�Ŵŵ���" << std::endl;

        if (CPU == 1)
        {
            std::cout << "���󣺽���1��Ϊϵͳ�����ڱ�ϵͳ�н���ʹ���κδ��̡�" << std::endl;
            return;
        }

        PCB *current_process = all_processes[CPU];
        hard_disks[disk_number]->Request(track_no, current_process->GetPid());

        // ��CPU���Ƴ���ǰ���̲��Ӿ���������ȡ���½���ִ��
        GetNextFromReadyQueue();
    }
    else
    {
        std::cout << "û��Ӳ��" << disk_number << std::endl;
    }
}

// ����������а����������̣���Ӿ���������ɾ���ý��̡� 
void OperatingSystem::RemoveFromReadyQueue(const int pid_)
{
    auto pid_to_delete = std::find(ready_queue.begin(), ready_queue.end(), pid_);
    // ����ھ��������У���ɾ������ 
    if (pid_to_delete != ready_queue.end())
    {
        ready_queue.erase(pid_to_delete);
    }
}

// ���ÿ��Ӳ�����Ƿ��и������̣�����ý�������ʹ�ô��̲�λ������һ�� IO�����ϣ���Ὣ��ɾ����
void OperatingSystem::RemoveFromDisks(const int &pid_)
{
    for (auto itr = hard_disks.begin(); itr != hard_disks.end(); itr++)
    {
        (*itr)->Remove(pid_);
    }
}

// ��ʾ��ǰʹ�� CPU �Ľ��̣����г����������е����н��̡�
void OperatingSystem::Snapshot() const
{
    std::cout << "ռ��CPU�Ľ��̣�" << CPU << std::endl;
    std::cout << "��������";
    for (auto itr = ready_queue.begin(); itr != ready_queue.end(); ++itr)
    {
        std::cout << " <- " << *itr;
    }
    std::cout << std::endl;
}

// ��ʾ�ڴ�״̬
// ����ÿ��ʹ�õ�ҳ����ʾռ�����Ľ��̺��Լ����д洢��ҳ��š�
// ҳ���ҳ��ı�Ŵ�0��ʼ
void OperatingSystem::MemorySnapshot()
{
    std::cout << "ҳ��\t\t"
              << "ҳ��\t\t"
              << "pid\t\t"
              << "ʱ���" << std::endl;
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

// ��ʾ��Щ���̵�ǰ����ʹ��Ӳ���Լ���Щ�������ڵȴ�ʹ������
void OperatingSystem::IOSnapshot() const
{
    for (int i = 0; i < number_of_hard_disks; i++)
    {
        std::cout << "Ӳ�� " << i << ": ";
        if (hard_disks[i]->DiskIsIdle())
        {
            std::cout << "����" << std::endl;
        }
        else
        {
            std::cout << "[" << hard_disks[i]->GetCurrentProcess() << " " << hard_disks[i]->GetCurrentTrack() << "]" << std::endl;
            std::cout << "Ӳ�� " << i << "��IO����: ";
            hard_disks[i]->PrintQueue();
        }
    }
}

// ��CPU���Ƴ���ǰ���̲��Ӿ���������ȡ���½���ִ�� 
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

// �����һ������IO�󣬽�����̷Żص����������С�
void OperatingSystem::RemoveProcessFromDisk(const int &disk_number)
{
    if ((disk_number < number_of_hard_disks) && (disk_number >= 0))
    {
        int removed_pcb = hard_disks[disk_number]->RemoveProcess();
        AddToReadyQueue(removed_pcb);
    }
    else
    {
        std::cout << "û��Ӳ��" << disk_number << std::endl;
    }
}

// �����������̵�ÿ��ҳ������ҵ��ý��̣�����ɾ����
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

// ɾ�����̵������ӽ��̣��������еĴ��̡�ҳ��IO���к;���������ɾ�����Ǽ���PCB��
void OperatingSystem::DeleteChildren(PCB *pcb)
{
    // ɾ����ǰ�Ľ��̵������ӽ��� 
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

            // ��PCB�����Ƴ��ӽ���
            all_processes.erase(child->GetPid());
            delete child;
            child = nullptr;
        }
    }

    pcb->ClearChildren();

    // ���ֵ�ǰ���̵Ľ���״̬���Ժ�ɾ��PCB
    if (!pcb->IsZombieProcess())
    {
        all_processes.erase(pcb->GetPid());
    }

    RemoveFromDisks(pcb->GetPid());
    RemoveFromFrames(pcb->GetPid());
    RemoveFromReadyQueue(pcb->GetPid());
}
