#include <string>
#include <sstream>
#include <iostream>

#include "disk.h"

using namespace std;

HardDisk::HardDisk() : direction(Inc), current_process(-1), current_track(0), io_queue(0) {}

HardDisk::~HardDisk() {}

// ���и���pid�Ľ�������ʹ�ô��̶�ȡ/д����Ϊtrack_no�Ĵŵ���
// ���մ��̵����㷨����ͬ���̵�IO�������ô��̵�IO�����С� 
void HardDisk::Request(const int track_no, const int &pid)
{
//    std::cout << "���д����ɾ��������䣺���и���pid�Ľ�������ʹ�ô��̶�ȡ/д����Ϊtrack_no�Ĵŵ���" << std::endl;

    // ������̿�����ֱ�ӷ��䣬�������IO����
    if (DiskIsIdle())
    {
        current_process = pid;
        current_track = track_no;
        direction = Inc; // ����ɸ���ʵ����Ҫ������ͷ���ƶ�����
    }
    else
    {
//        io_queue.emplace_back(pid, track_no); // ���ڴ洢���̵�IO���󣬼�¼���̵�PID�Ͷ�Ӧ�Ĵŵ���
        // SCAN���룺���ݵ�ǰdirection��track_no�������
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

// ����ǰʹ�ô��̵Ľ��̴Ӵ����Ƴ����Żؾ������С�
// ���IO�����л������������ڵȴ��ô��̣����մ��̵����㷨ѡ��һ������ʹ�øô��̡�
int HardDisk::RemoveProcess()
{
//    std::cout << "���д����ɾ��������䣺����ǰʹ�ô��̵Ľ��̴Ӵ����Ƴ����Żؾ������С�" << std::endl;

    int removed_process = current_process;
    current_process = -1;

    // �� IO ���в��գ���Ӷ��׻�ȡ��һ������ʹ�ô���
    if (!io_queue.empty())
    {
        auto next_request = io_queue.front();
        io_queue.pop_front();
        current_process = next_request.first;
        current_track = next_request.second;
        direction = Inc; // �ɸ��ݵ����㷨��������
    }

    return removed_process;
}

// �Ƴ�����ʹ�ô��̻���IO�����еĸ������̡�
void HardDisk::Remove(const int &pid)
{
    // ����������ʹ�ô��� 
    if (current_process == pid)
    {
        RemoveProcess();
    }
    // ��������IO������ 
    for (auto itr = io_queue.begin(); itr != io_queue.end(); itr++)
    {
        if ((*itr).first == pid)
        {
            io_queue.erase(itr);
        }
    }
}

// ������ʹ�ô��̵Ľ��̵�PID��
int HardDisk::GetCurrentProcess() const
{
    return current_process;
}

// ���ص�ǰ�������ڶ�ȡ��д��ĵ�ǰ�ŵ��š� 
int HardDisk::GetCurrentTrack() const
{
    return current_track;
}

// ���õ�ǰʹ�ô��̵Ľ��̡�
void HardDisk::SetCurrentProcess(const int &pid)
{
    current_process = pid;
}

// ���Դ��̵�ǰ�Ƿ���С�
bool HardDisk::DiskIsIdle() const
{
    return current_process == -1;
}

// �����ǰ���̵�IO���С�
void HardDisk::PrintQueue() const
{
    for (auto itr = io_queue.begin(); itr != io_queue.end(); itr++)
    {
        std::cout << "<- [" << (*itr).first << " " << (*itr).second << "] ";
    }
    std::cout << std::endl;
}
