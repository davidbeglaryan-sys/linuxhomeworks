#include <iostream>
#include <unistd.h>     
#include <signal.h>     
#include <sys/types.h>  
#include <pwd.h>        
#include <string.h>    
#include <ucontext.h>   


const char* get_username_by_uid(uid_t uid) {
    struct passwd *pw;
    pw = getpwuid(uid);
    return (pw) ? pw->pw_name : "Unknown User";
}


void sig_handler(int signo, siginfo_t *info, void *context) {
    pid_t sender_pid = info->si_pid;
    uid_t sender_uid = info->si_uid;
    const char* sender_username = get_username_by_uid(sender_uid);

    ucontext_t *uc = (ucontext_t *)context;


    unsigned long rip_val = 0;
    unsigned long rax_val = 0;
    unsigned long rbx_val = 0;
    
    #ifdef __x86_64__
        rip_val = uc->uc_mcontext.gregs[REG_RIP];
        rax_val = uc->uc_mcontext.gregs[REG_RAX];
        rbx_val = uc->uc_mcontext.gregs[REG_RBX];
    #elif __i386__
        rip_val = uc->uc_mcontext.gregs[REG_EIP];
        rax_val = uc->uc_mcontext.gregs[REG_EAX];
        rbx_val = uc->uc_mcontext.gregs[REG_EBX];
    #else

    #endif


    std::cout << "\n--- SIGNAL RECEIVED ---" << std::endl;
    std::cout << "Received a SIGUSR1 signal from process [" << sender_pid 
              << "] executed by [" << sender_uid << "] (";
    std::cout << sender_username << ")." << std::endl;

    printf("State of the context: EIP = 0x%lx, EAX = 0x%lx, EBX = 0x%lx.\n", 
           rip_val, rax_val, rbx_val);
    std::cout << "-----------------------" << std::endl;
}

int main() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa)); 
    
    sa.sa_sigaction = sig_handler; 
    sa.sa_flags = SA_SIGINFO;      
    
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Error with sigaction");
        return 1;
    }

    pid_t my_pid = getpid();
    std::cout << "Signal Echo program started." << std::endl;
    std::cout << "My PID : " << my_pid << std::endl;
    std::cout << "To send a signal, use the command (in another terminal):\n"
              << "kill -SIGUSR " << my_pid << std::endl;
    std::cout << "\n" << std::endl;

    while (true) {
        std::cout << "Sleeping for 10 seconds..." << std::endl;
        sleep(10);
    }

    return 0; 
}
