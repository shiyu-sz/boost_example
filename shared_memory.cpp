
#include <iostream>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

void interprocess_example_write() {
    std::cout << "\n=== 进程间内存管理示例 ===" << std::endl;
    
    using namespace boost::interprocess;
    
    // 创建共享内存
    struct shared_data {
        int counter;
        char data[100];
    };
    
    shared_memory_object::remove("boost_shared_memory");
    // 尝试创建共享内存
    shared_memory_object shm(
        create_only,                  // 只创建
        "boost_shared_memory",        // 名称
        read_write                    // 读写权限
    );
    
    // 设置大小
    shm.truncate(sizeof(shared_data));
    
    // 映射到进程地址空间
    mapped_region region(shm, read_write);
    
    // 获取映射区域的地址
    void* addr = region.get_address();
    
    // 使用 placement new 构造对象
    shared_data* data = new(addr) shared_data;
    data->counter = 0;
    strcpy(data->data, "Hello from process");
    
    std::cout << "共享内存已创建，地址: " << addr << std::endl;
    std::cout << "数据: " << data->data << ", 计数器: " << data->counter << std::endl;
    
    // 注意：在实际应用中，需要在另一个进程中打开这个共享内存
    // 可以通过相同的名称打开：open_only

    while(1) {
        data->counter ++;
        std::cout << "更新计数器: " << data->counter << std::endl;
        sleep(1);
    }
}

void interprocess_example_read() {
    std::cout << "\n=== 进程间内存管理示例 ===" << std::endl;
    
    using namespace boost::interprocess;
    
    // 创建共享内存
    struct shared_data {
        int counter;
        char data[100];
    };
    
    // 尝试创建共享内存
    shared_memory_object shm(
        open_only,                  // 只打开
        "boost_shared_memory",        // 名称
        read_only                    // 读写权限
    );

    // 映射到进程地址空间
    mapped_region region(shm, read_only);

    // 获取映射区域的地址
    void* addr = region.get_address();

    // 使用 placement new 构造对象
    shared_data* data = new(addr) shared_data;
    
    std::cout << "共享内存已创建，地址: " << addr << std::endl;
    std::cout << "数据: " << data->data << ", 计数器: " << data->counter << std::endl;
    
    // 注意：在实际应用中，需要在另一个进程中打开这个共享内存
    // 可以通过相同的名称打开：open_only

    while(1) {
        std::cout << "计数器: " << data->counter << std::endl;
        sleep(2);
    }
}

int main(int argc, char **argv)
{
    if(argv[1][0] == 'w') {
        interprocess_example_write();
    } else {
        interprocess_example_read();
    }
}