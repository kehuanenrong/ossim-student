#ifndef PCB_H
#define PCB_H

#include <vector>


class PCB {
    public:

        PCB(int & pid_);
        
        ~PCB();
        
        int GetPid() const;

        // Adds a pointer to a child process of this pcb
        void AddChildProcess(PCB* child_process);

        // Sets the process to waiting or not waiting
        void SetWaitingState(bool state);
        
        // Marks the process as a zombie (1) or not a zombie (0)
        void SetZombie(bool state);

        // Returns true if the process is waiting, otherwise returns false
        bool IsWaiting();

        // Returns true if the process is a zombie process. Returns false otherwise
        bool IsZombieProcess() const;

        // If the process has a child that is a zombie, returns the pid of that child. If it does not, returns 0.
        int ProcessHasZombieChild() const;

        //Returns true if the process has children.
        bool HasChildren() const;
        
        // When called on a process, removes the given child if it is found in child_processes.
        void RemoveChild(PCB* & child);

        // Sets the parent pid of a process
        void SetParent(const int parent_pid);

        // Returns the parent pid of a process
        int GetParent();

        // Returns a constant reference to the vector of child processes.
        const std::vector<PCB*> & GetChildren() const;

        // Clears the vector of child processes.
        void ClearChildren();
        
        
    private:
        int pid;                                // Unique id of the process
        std::vector<PCB*> child_processes;      // Pointers of all children of the process
        int parent_process;                     // The pid of the parent of the process
        bool process_is_zombie;                 // True is process is a zombie process, false otherwise
        bool waiting;                           // True if this process is waiting for a child process to terminate.
};

#endif // PCB_H
