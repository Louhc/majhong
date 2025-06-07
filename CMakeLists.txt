cmake_minimum_required(VERSION 3.10)
project(Mahjong CXX) # 指定项目语言为 CXX喵

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 强制要求 C++17 标准喵
set(CMAKE_CXX_EXTENSIONS OFF)      # 禁用 GNU 扩展 保持代码更可移植喵

# 设置构建类型默认值 如果未指定喵
# if (NOT CMAKE_BUILD_TYPE)
#     set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel")
# endif()

# 设置一个变量来存储所有源代码目录喵
# 这样可以方便地管理 include 路径和源文件查找喵
set(PROJECT_SOURCE_DIRS
    src/core
    src/game
    src/display
)

# 添加所有头文件目录喵
foreach(DIR ${PROJECT_SOURCE_DIRS})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${DIR})
endforeach()
# 额外添加 src 根目录 因为 main.cpp 可能在那里喵
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# --- 编译 main.cpp 为可执行文件 'MahjongGame' (对应 make build) ---
# 定义 main.cpp 所在目录和文件名
set(MAIN_SOURCE_DIR src)
set(MAIN_SOURCE ${MAIN_SOURCE_DIR}/main.cpp) # 假设主程序入口是 src/main.cpp

# 递归查找所有非测试相关的源文件喵
file(GLOB_RECURSE GAME_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/display/*.cpp"
)

# 添加主程序的源文件到列表喵
list(APPEND GAME_SOURCES ${MAIN_SOURCE})

add_executable(MahjongGame ${GAME_SOURCES}) # MahjongGame 是你的主程序名喵
# 设置主程序的目标属性喵
target_compile_options(MahjongGame PRIVATE -Wall -Wextra) # 推荐添加更多警告喵

# --- 编译测试 (对应 make test_name) ---
# 启用测试喵
enable_testing()

# 查找所有测试文件
file(GLOB_RECURSE TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_*.cpp")

# 为每个测试文件创建一个单独的可执行目标和测试喵
foreach(TEST_FILE ${TEST_FILES})
    # 从完整路径中提取测试名称喵
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE) # 例如：test_hand
    
    # 为每个测试创建可执行文件喵
    add_executable(${TEST_NAME} ${TEST_FILE} ${GAME_SOURCES}) # 注意：这里为了简单 测试也链接了所有游戏源码。更精细的做法是只链接测试需要的模块喵。
    
    # 添加测试到 CTest喵
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    
    # 可以为测试目标设置特定的编译选项喵
    target_compile_options(${TEST_NAME} PRIVATE -DTEST_BUILD) # 例如 定义宏以便测试代码知道它正在测试构建喵
endforeach()


# --- make all 时编译全部 ---
# 默认情况下 `add_executable` 和 `add_library` 定义的目标都会在 `make all` 时被编译喵。
# 所以你不需要额外做什么 所有你定义的 `add_executable` 目标都会被编译喵。