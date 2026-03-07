# AutoRHI

AutoRHI is a cross-platform Rendering Hardware Interface library that provides a unified API for different graphics APIs, making it easier to develop graphics applications that can run on multiple platforms with minimal code changes.

## Features

- Supported backends: OpenGL, Vulkan, WebGPU
- Lightweight with no third-party dependencies in the core part
- Uses reflection to generate pipeline layouts, avoiding tedious manual code
- Uses names to bind resources, more intuitive than indices
- Compile shaders at runtime

## Building

[Third-party dependencies](https://gitee.com/chunquedong/third-party)

```bash
fmake fmake_simple.props
```
