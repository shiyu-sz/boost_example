#include <boost/smart_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include <vector>
#include <boost/pool/pool.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

// 用于演示 intrusive_ptr 的类，内置引用计数
class IntrusiveClass {
private:
    long ref_count;  // 内置引用计数
    int value;
    
public:
    IntrusiveClass(int v) : ref_count(0), value(v) {
        std::cout << "IntrusiveClass 构造，值: " << value << std::endl;
    }
    
    ~IntrusiveClass() {
        std::cout << "IntrusiveClass 析构，值: " << value << std::endl;
    }
    
    int getValue() const { return value; }
    
    // 以下两个函数必须提供给 boost::intrusive_ptr 使用
    friend void intrusive_ptr_add_ref(IntrusiveClass* p) {
        ++p->ref_count;
        std::cout << "引用计数增加，当前: " << p->ref_count << std::endl;
    }
    
    friend void intrusive_ptr_release(IntrusiveClass* p) {
        if (--p->ref_count == 0) {
            std::cout << "引用计数归零，删除对象" << std::endl;
            delete p;
        } else {
            std::cout << "引用计数减少，当前: " << p->ref_count << std::endl;
        }
    }
};

// 演示 shared_from_this 的类
class Node : public boost::enable_shared_from_this<Node> {
public:
    std::string name;
    boost::shared_ptr<Node> next;
    
    explicit Node(const std::string& n) : name(n) {
        std::cout << "Node " << name << " 构造" << std::endl;
    }
    
    ~Node() {
        std::cout << "Node " << name << " 析构" << std::endl;
    }
    
    boost::shared_ptr<Node> getShared() {
        return shared_from_this();  // 安全地获得指向自己的 shared_ptr
    }
    
    void printChain() {
        std::cout << name;
        if (next) {
            std::cout << " -> ";
            next->printChain();
        }
    }
};

void pool_example() {
    std::cout << "\n=== pool 示例 ===" << std::endl;
    
    // 创建一个每次分配 32 字节的内存池
    boost::pool<> pool(32);
    
    // 从池中分配内存
    void* p1 = pool.malloc();
    void* p2 = pool.malloc();
    
    std::cout << "从内存池分配了两个 32 字节的块" << std::endl;
    std::cout << "p1 地址: " << p1 << std::endl;
    std::cout << "p2 地址: " << p2 << std::endl;
    
    // 归还内存
    pool.free(p1);
    pool.free(p2);
    
    // 批量分配
    void* p3 = pool.ordered_malloc(5);  // 一次分配 5 个块
    std::cout << "批量分配 5 个块，起始地址: " << p3 << std::endl;
    
    pool.ordered_free(p3, 5);
}

class Point {
public:
    Point(int x = 0, int y = 0) : x_(x), y_(y) {
        std::cout << "Point(" << x_ << "," << y_ << ") 被构造" << std::endl;
    }
    ~Point() {
        std::cout << "Point(" << x_ << "," << y_ << ") 被析构" << std::endl;
    }
    void display() const {
        std::cout << "Point: (" << x_ << ", " << y_ << ")" << std::endl;
    }
private:
    int x_, y_;
};

void object_pool_example() {
    std::cout << "\n=== object_pool 示例 ===" << std::endl;
    
    // 创建 Point 对象池
    boost::object_pool<Point> pool;
    
    // 构造对象（自动调用构造函数）
    Point* p1 = pool.construct(10, 20);
    Point* p2 = pool.construct(30, 40);
    
    p1->display();
    p2->display();
    
    // 也可以直接 malloc 然后使用（不调用构造函数）
    Point* p3 = pool.malloc();
    // 注意：此时 p3 指向的内存未初始化，需要手动调用构造函数
    new(p3) Point(50, 60);  // placement new
    p3->display();
    
    // 不需要手动释放，对象池销毁时会自动释放所有内存
}

class Resource {
public:
    explicit Resource(const std::string& name) : name_(name) {
        std::cout << "Resource " << name_ << " 被创建" << std::endl;
    }
    ~Resource() {
        std::cout << "Resource " << name_ << " 被销毁" << std::endl;
    }
    void use() const {
        std::cout << "使用资源: " << name_ << std::endl;
    }
private:
    std::string name_;
};

void ptr_container_example() {
    std::cout << "\n=== ptr_vector 示例 ===" << std::endl;
    
    boost::ptr_vector<Resource> resources;
    
    // 直接添加动态分配的对象（容器接管所有权）
    resources.push_back(new Resource("R1"));
    resources.push_back(new Resource("R2"));
    resources.push_back(new Resource("R3"));
    
    std::cout << "容器大小: " << resources.size() << std::endl;
    
    // 像普通容器一样使用
    for (size_t i = 0; i < resources.size(); ++i) {
        resources[i].use();  // 直接使用对象，不是指针
    }
    
    // 自动遍历
    for (const auto& res : resources) {
        res.use();
    }
    
    // 移除元素时会自动删除对象
    resources.pop_back();
    std::cout << "pop_back 后大小: " << resources.size() << std::endl;
    
    // 容器销毁时会自动释放所有资源
}

int main() {
    #if 0
    std::cout << "=== 1. scoped_ptr 演示 ===" << std::endl;
    {
        boost::scoped_ptr<int> p(new int(100));
        std::cout << "scoped_ptr 管理的值: " << *p << std::endl;
        // p 离开作用域时自动释放内存
    }
    std::cout << "scoped_ptr 已自动释放" << std::endl;
    #endif

    #if 0
    std::cout << "\n=== 2. shared_ptr 演示 ===" << std::endl;
    {
        boost::shared_ptr<Node> node1(new Node("Node1"));
        boost::shared_ptr<Node> node2 = node1;  // 引用计数增加
        boost::shared_ptr<Node> node3 = node2;  // 引用计数增加
        
        std::cout << "use_count: " << node1.use_count() << std::endl;
        // 3个 shared_ptr 共享同一个对象
    }  // 当最后一个 shared_ptr 销毁时，Node 才被析构
    std::cout << "所有 shared_ptr 已销毁" << std::endl;
    #endif

    #if 0
    std::cout << "\n=== 3. weak_ptr 演示（解决循环引用）===" << std::endl;
    {
        boost::shared_ptr<Node> a(new Node("A"));
        boost::shared_ptr<Node> b(new Node("B"));
        
        // 建立循环引用
        a->next = b;
        b->next = a;  // 这里会出问题！形成循环引用
        
        std::cout << "a use_count: " << a.use_count() << std::endl;  // 2 (a 和 b->next)
        std::cout << "b use_count: " << b.use_count() << std::endl;  // 2 (b 和 a->next)
    }  // 内存泄漏！两个 Node 都不会被析构
    #endif
    
    #if 0
    // 正确的做法：使用 weak_ptr
    class CorrectNode : public boost::enable_shared_from_this<CorrectNode> {
    public:
        std::string name;
        boost::weak_ptr<CorrectNode> next;  // 使用 weak_ptr 打破循环
        
        explicit CorrectNode(const std::string& n) : name(n) {}
        ~CorrectNode() { std::cout << "CorrectNode " << name << " 析构" << std::endl; }
        
        void print() {
            std::cout << name;
            auto sp = next.lock();  // 尝试获得 shared_ptr
            if (sp) {
                std::cout << " -> ";
                sp->print();
            }
        }
    };
    
    {
        boost::shared_ptr<CorrectNode> a(new CorrectNode("A"));
        boost::shared_ptr<CorrectNode> b(new CorrectNode("B"));
        
        a->next = b;
        b->next = a;  // 现在不会增加引用计数
        
        std::cout << "a use_count: " << a.use_count() << std::endl;  // 1
        std::cout << "b use_count: " << b.use_count() << std::endl;  // 1
        
        // std::cout << "链: ";
        // a->print();
        // std::cout << std::endl;
    }  // 两个对象都能正确析构
    #endif
    
    #if 0
    std::cout << "\n=== 4. intrusive_ptr 演示 ===" << std::endl;
    {
        boost::intrusive_ptr<IntrusiveClass> p1(new IntrusiveClass(42));
        {
            boost::intrusive_ptr<IntrusiveClass> p2 = p1;  // 引用计数增加
            std::cout << "p1 指向的值: " << p1->getValue() << std::endl;
            std::cout << "p2 指向的值: " << p2->getValue() << std::endl;
        }  // p2 析构，引用计数减少但不为0
        std::cout << "p1 仍然有效" << std::endl;
    }  // p1 析构，引用计数归零，对象被删除
    #endif
    
    // pool_example();      // 内存池
    // object_pool_example();   // 对象池
    ptr_container_example();    // 指针容器

    return 0;
}