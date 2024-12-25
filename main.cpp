#include <string>
#include <sstream>
#include <iostream>

#include "PCB.h"
#include "disk.h"
#include "OS.h"

using namespace std;

int main() {

    unsigned int RAM = 0;
    unsigned int page_size = 0;
    int number_of_hard_disks = 0;

    std::cout << "΢���������ϵͳ" << std::endl;
	std::cout << "�������ڴ����������ֽ�Ϊ��λ����" << std::endl;
    std::cin >> RAM;

    std::cout << "������ҳ��/ҳ���С�����ֽ�Ϊ��λ����" << std::endl;
    std::cin >> page_size;

    std::cout << "������������������Ӳ�̸�����" << std::endl;
    std::cin >> number_of_hard_disks;


    OperatingSystem OS(number_of_hard_disks, RAM, page_size);

    std::string input;
    std::getline(std::cin, input);

    while (1) {
         // ��ʾ��ǰ�ĸ���������ʹ��CPU�Լ���Щ�������ھ��������еȴ��� 
        if (input == "show p") {
            OS.Snapshot();
        }
        // ��ʾ��Щ���̵�ǰ����ʹ��Ӳ���Լ���Ӳ�̵�IO���С� 
        else if (input == "show i") {
            OS.IOSnapshot();
        }
        // ��ʾ�ڴ�״̬�� 
        else if (input == "show m") {
            OS.MemorySnapshot();
        }
        // ����һ���µ�PCB������������ھ������е�ĩβ�������������Ϊ�գ�������ʹ��CPU��
        else if (input == "new") {
            OS.CreateProcess();
        }
        // �ǰ����ʹ��CPU����һ�����ġ�
        else if (input == "-") {
            OS.CPUToReadyQueue();
        }
        // ʹ��CPU�Ľ��̴���һ���ӽ��̣����ӽ��̱������ھ������е�ĩβ��
        else if (input == "fork") {
            OS.Fork();
        }
        // ��ǰ������ֹ�� 
        else if (input == "exit") {
            OS.Exit();
        }
        // ��ǰ������ͣ���ȴ����ӽ�����ֹ��
        else if (input == "wait") {
            OS.Wait();
        }else if (input == "quit"){	exit(0); }
        
        // �������� 
        std::stringstream in_stream(input);
        string first_word;
        in_stream >> first_word;

        if (first_word == "d" || first_word == "D" || first_word == "m") {
            int second_word;
            in_stream >> second_word;
            //��ǰʹ��CPU�Ľ�������Ӳ��#number����ȡ��д����Ϊ#track_no�Ĵŵ���
            if (first_word == "d") {
                int third_word;
                in_stream >> third_word;
                OS.RequestDisk(second_word, third_word);
            }
            else if (first_word == "D") { // "D number") {
                // Ӳ��#number�����һ�����̵�IO���� 
                OS.RemoveProcessFromDisk(second_word);
            }
            //��ǰ����ʹ��CPU�Ľ��������ĳ�߼���ַ�����ڴ������
            else if (first_word == "m") { // == "m address") {
                OS.RequestMemoryOperation(second_word);
            }
        }       
        // ��ȡ�û�����һ�����롣 
        std::cout << endl;
        std::getline(std::cin, input);
    }
}
