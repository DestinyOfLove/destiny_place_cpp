add_library(message_queue_lib INTERFACE
        single_thread_queue.h
        ring_buffer_queue.h)
target_include_directories(message_queue_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})