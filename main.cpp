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

    std::cout << "微型虚拟操作系统" << std::endl;
	std::cout << "请输入内存容量（以字节为单位）：" << std::endl;
    std::cin >> RAM;

    std::cout << "请输入页面/页框大小（以字节为单位）：" << std::endl;
    std::cin >> page_size;

    std::cout << "请输入所仿真计算机的硬盘个数：" << std::endl;
    std::cin >> number_of_hard_disks;


    OperatingSystem OS(number_of_hard_disks, RAM, page_size);

    std::string input;
    std::getline(std::cin, input);

    while (1) {
         // 显示当前哪个进程正在使用CPU以及哪些进程正在就绪队列中等待。 
        if (input == "show p") {
            OS.Snapshot();
        }
        // 显示哪些进程当前正在使用硬盘以及各硬盘的IO队列。 
        else if (input == "show i") {
            OS.IOSnapshot();
        }
        // 显示内存状态。 
        else if (input == "show m") {
            OS.MemorySnapshot();
        }
        // 创建一个新的PCB，并将其放置在就绪队列的末尾，如果就绪队列为空，则让其使用CPU。
        else if (input == "new") {
            OS.CreateProcess();
        }
        // 令当前进程使用CPU运行一个节拍。
        else if (input == "-") {
            OS.CPUToReadyQueue();
        }
        // 使用CPU的进程创建一个子进程，该子进程被放置在就绪队列的末尾。
        else if (input == "fork") {
            OS.Fork();
        }
        // 当前进程终止。 
        else if (input == "exit") {
            OS.Exit();
        }
        // 当前进程暂停并等待其子进程终止。
        else if (input == "wait") {
            OS.Wait();
        }else if (input == "quit"){	exit(0); }
        
        // 其他命令 
        std::stringstream in_stream(input);
        string first_word;
        in_stream >> first_word;

        if (first_word == "d" || first_word == "D" || first_word == "m") {
            int second_word;
            in_stream >> second_word;
            //当前使用CPU的进程请求硬盘#number，读取或写入编号为#track_no的磁道。
            if (first_word == "d") {
                int third_word;
                in_stream >> third_word;
                OS.RequestDisk(second_word, third_word);
            }
            else if (first_word == "D") { // "D number") {
                // 硬盘#number已完成一个进程的IO请求。 
                OS.RemoveProcessFromDisk(second_word);
            }
            //当前正在使用CPU的进程请求对某逻辑地址进行内存操作。
            else if (first_word == "m") { // == "m address") {
                OS.RequestMemoryOperation(second_word);
            }
        }       
        // 获取用户的下一行输入。 
        std::cout << endl;
        std::getline(std::cin, input);
    }
}
