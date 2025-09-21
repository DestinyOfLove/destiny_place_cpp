# TODO / Cleanup Plan

- [x] 移除 Windows 专用代码路径，统一采用 mmap，失败时直接抛出异常。
- [x] 重构 SeriesData，移除 cursor/use of cursor factory，改由简单数据结构承载 string_view 与整数缓冲。
- [x] 精简 SeriesComparatorFactory，改为直接使用 switch/静态函数。

