#include <string>
#include <stdexcept>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

class SharedArray {
private:
    std::string name_;
    size_t size_;
    int shm_fd_ = -1;
    int* data_ = nullptr;
    sem_t* semaphore_ = SEM_FAILED;

    static constexpr mode_t SHM_PERMS = 0660;
    static constexpr mode_t SEM_PERMS = 0666;
    static constexpr int INITIAL_SEM_VALUE = 1;

    void cleanup() {
        if (data_ != MAP_FAILED && data_ != nullptr) {
            munmap(data_, size_ * sizeof(int));
        }
        if (semaphore_ != SEM_FAILED) {
            sem_close(semaphore_);
        }
        if (shm_fd_ != -1) {
            close(shm_fd_);
        }
    }

public:
    SharedArray(const std::string& name, size_t size) : name_(name), size_(size) {
        if (size_ == 0 || size_ > 1000000000) {
            throw std::out_of_range("Invalid array size.");
        }

        std::string shm_name = "/" + name_ + "_shm";
        std::string sem_name = "/" + name_ + "_sem";
        size_t total_size = size_ * sizeof(int);

        shm_fd_ = shm_open(shm_name.c_str(), O_CREAT | O_RDWR, SHM_PERMS);
        if (shm_fd_ == -1) {
            throw std::runtime_error("shm_open failed.");
        }

        /
        struct stat shm_stat;
        if (fstat(shm_fd_, &shm_stat) == -1) {
            close(shm_fd_);
            throw std::runtime_error("fstat failed.");
        }

        if (shm_stat.st_size == 0) {
            if (ftruncate(shm_fd_, total_size) == -1) {
                close(shm_fd_);
                throw std::runtime_error("ftruncate failed.");
            }
        } else if ((size_t)shm_stat.st_size != total_size) {
            cleanup();
            throw std::runtime_error("Name collision: existing shared array has a different size.");
        }

        data_ = (int*)mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_, 0);
        if (data_ == MAP_FAILED) {
            cleanup();
            throw std::runtime_error("mmap failed.");
        }

        // 4. Semaphore Setup
        semaphore_ = sem_open(sem_name.c_str(), O_CREAT, SEM_PERMS, INITIAL_SEM_VALUE);
        if (semaphore_ == SEM_FAILED) {
            cleanup();
            throw std::runtime_error("sem_open failed.");
        }
    }

    ~SharedArray() {
        cleanup();
    }

    SharedArray(const SharedArray&) = delete;
    SharedArray& operator=(const SharedArray&) = delete;
    SharedArray(SharedArray&&) = delete;
    SharedArray& operator=(SharedArray&&) = delete;

    int& operator[](size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of bounds.");
        }
        return data_[index];
    }

    void lock() {
        if (sem_wait(semaphore_) == -1) {
            throw std::runtime_error("sem_wait failed.");
        }
    }

    void unlock() {
        if (sem_post(semaphore_) == -1) {
            throw std::runtime_error("sem_post failed.");
        }
    }

    static void unlink_resources(const std::string& name) {
        std::string shm_name = "/" + name + "_shm";
        std::string sem_name = "/" + name + "_sem";
        shm_unlink(shm_name.c_str());
        sem_unlink(sem_name.c_str());
    }
};
