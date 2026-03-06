# 高级Makefile，包含Boost库检测
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
LDFLAGS = 

# Boost库检测
BOOST_LIBS = -lboost_system -lboost_timer -lpthread
BOOST_INCLUDE = /usr/include/boost
BOOST_LIB_PATH = /usr/lib

# 检查Boost是否安装
CHECK_BOOST = $(shell pkg-config --exists boost && echo yes)
ifeq ($(CHECK_BOOST),yes)
    BOOST_CFLAGS = $(shell pkg-config --cflags boost)
    BOOST_LIBS = $(shell pkg-config --libs boost) -lboost_system -lboost_timer -lpthread
else
    # 手动设置路径（如果需要）
    BOOST_CFLAGS = -I$(BOOST_INCLUDE)
    BOOST_LDFLAGS = -L$(BOOST_LIB_PATH)
endif

# 源文件
SOURCES = 2.timer.cpp 3.memory.cpp shared_memory.cpp
TARGETS = $(SOURCES:.cpp=)

# 颜色输出
GREEN = \033[0;32m
RED = \033[0;31m
NC = \033[0m

# 检测是否存在源文件
EXISTING_SOURCES = $(wildcard $(SOURCES))
EXISTING_TARGETS = $(EXISTING_SOURCES:.cpp=)

# 默认目标
all: check_boost $(EXISTING_TARGETS)
	@echo "$(GREEN)编译完成！$(NC)"

# 通用编译规则
%: %.cpp
	@echo "编译 $<..."
	$(CXX) $(CXXFLAGS) $(BOOST_CFLAGS) -o $@ $< $(BOOST_LDFLAGS) $(BOOST_LIBS)

# 清理
clean:
	@echo "清理文件..."
	rm -f $(EXISTING_TARGETS) *.o
	@echo "$(GREEN)清理完成$(NC)"

# 运行所有示例
run: all
	@echo "运行同步定时器示例..."
	@./sync_timer || true
	@echo ""
	@echo "运行异步定时器示例..."
	@./async_timer || true
	@echo ""
	@echo "运行重复定时器示例..."
	@./repeating_timer || true

# 调试版本
debug: CXXFLAGS += -DDEBUG -g -O0
debug: all
	@echo "$(GREEN)调试版本编译完成$(NC)"

# 性能分析版本
profile: CXXFLAGS += -pg
profile: all
	@echo "$(GREEN)性能分析版本编译完成$(NC)"

# 帮助
help:
	@echo "使用方法: make [目标]"
	@echo ""
	@echo "可用目标:"
	@echo "  all     - 编译所有示例（默认）"
	@echo "  clean   - 清理编译文件"
	@echo "  run     - 编译并运行所有示例"
	@echo "  debug   - 编译调试版本"
	@echo "  profile - 编译性能分析版本"
	@echo "  help    - 显示此帮助"
	@echo ""
	@echo "可编译的示例:"
	@for source in $(EXISTING_SOURCES); do \
		echo "  $$source -> $${source%.cpp}"; \
	done

.PHONY: all clean run debug profile help check_boost