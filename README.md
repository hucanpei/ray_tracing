## 简介
《[ray tracing in one weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)》的代码，加上TBB实现多线程

实现光线追踪，主要算法是path tracing

## 使用
- 编译：
```
$ g++ ./main.cc -o main -ltbb
```
- 运行
```
$ main > demo.ppm
```

## 最终效果
![demo](demo.png)
