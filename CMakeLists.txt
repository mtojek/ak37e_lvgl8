# cmake最低版本号要求
cmake_minimum_required(VERSION 2.8)
# 项目名称
project(ak37e_lvgl8 LANGUAGES C)
# 选择要使用的编译器，请确保系统环境变量包含编译器路径
set(CMAKE_C_COMPILER "arm-anykav500-linux-uclibcgnueabi-gcc")
# 设置编译选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
# 添加需要链接的库文件目录
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/ak37e/lib")
# 查找目录下的所有源文件，并将名称保存到 *_SRCS 变量
aux_source_directory(. MAIN_SRCS)
file(GLOB_RECURSE AK37E_SRCS ak37e/*.c)
file(GLOB_RECURSE LVGL_SRCS lvgl/*.c)
file(GLOB_RECURSE WORK_SRCS work/*.c)
# 指定生成目标，链接库
add_executable(ak37e_lvgl_demo ${LVGL_SRCS} ${AK37E_SRCS} ${WORK_SRCS} ${MAIN_SRCS})
target_link_libraries(
ak37e_lvgl_demo
-Wl,--start-group
libplat_ao.a libplat_ai.a libplat_vqe.a libakaudiofilter.a libplat_common.a libplat_log.a libplat_dbg.a
libplat_mem.a libpthread.a libplat_thread.a libplat_vo.a libplat_tde.a libplat_osal.a
-Wl,--end-group
)
