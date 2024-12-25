#ifndef DISK_H
#define DISK_H

#include <list>
#include <string>

// ���̴�ͷ�����з���Inc--�ŵ������ӵķ���Dec--�ŵ��ż�С�ķ���
enum Direction {
	Inc,
	Dec
};

class HardDisk {
    public:
        HardDisk();
        
        ~HardDisk();
        
        // ���и���pid�Ľ�������ʹ�ô��̶�ȡ/д����Ϊ track_no �Ĵŵ���
		// ���մ��̵����㷨����ͬ���̵�IO�������ô��̵�IO�����С� 
		void Request(const int track_no, const int &pid);

        // ����ǰʹ�ô��̵Ľ��̴Ӵ����Ƴ����Żؾ������С�
		// ���IO�����л������������ڵȴ��ô��̣����մ��̵����㷨ѡ��һ������ʹ�øô��̡�
        int RemoveProcess();

        // �Ƴ�����ʹ�ô��̻���IO�����еĸ������̡�
        void Remove(const int & pid);

        // ������ʹ�ô��̵Ľ��̵�PID��
        int GetCurrentProcess() const;
        
        // ���ص�ǰ�������ڶ�ȡ��д��ĵ�ǰ�ŵ��š�
        int GetCurrentTrack() const;

        // ���õ�ǰʹ�ô��̵Ľ��̡�
        void SetCurrentProcess(const int & pid);

        // ���Դ��̵�ǰ�Ƿ���С�
        bool DiskIsIdle() const;

        // �����ǰ���̵�IO���С�
        void PrintQueue() const;

    private:
		Direction direction;									// ��ǰ���̴�ͷ�����з���
        int current_process;                                    // ��ǰʹ�ô��̵Ľ���PID�����Ϊ-1���ʾ���̿��С�
        int current_track;										// ��ǰ���ڶ�ȡ��д��Ĵŵ��š�
        std::list<std::pair<int, int> > io_queue;        		// ���̵�IO���У�����ÿ��Ԫ��Ϊһ��IO���󣬰����˽��̵�PID�ʹŵ��š�
};

#endif // DISK_H
