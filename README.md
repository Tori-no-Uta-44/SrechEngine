# MyMicroservicesProject

## 项目结构
- `services/`: 包含每个独立的微服务
- `libs/`: 包含公共库

## 构建步骤
1. 在x目录下执行
    ```bash
    protoc --proto_path=<path> --cpp_out=<path> <idl_file>
    eg:protoc --proto_path=. --cpp_out=. x.proto
    srpc_generator protobuf <idl_file> <output_dir>
    eg:srpc_generator protobuf signup.proto .
2. 在项目根目录下创建构建目录并进入：
   ```bash
   mkdir build && cd build
   
3. 
