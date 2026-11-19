**Multithreaded In-Memory Key-Value Storage Engine (C++20)**

A high-performance, multithreaded in-memory key-value storage engine designed to provide fast data access through a Redis-compatible interface.

The system is designed around a TCP-based client-server architecture, allowing multiple clients to connect concurrently and execute commands for storing, retrieving, updating, and deleting key-value pairs. Client-server communication follows the RESP (Redis Serialization Protocol), providing compatibility with Redis clients and tools.

The server is designed for Linux and leverages POSIX APIs and system calls for network communication, process and thread management, and low-level system interaction. Its multithreaded architecture is intended to support concurrent client connections and request processing while maintaining data consistency and thread safety.

The engineering infrastructure includes a CMake-based build system, unit testing, debugging, and performance and memory profiling. Planned improvements include Docker-based containerization, CI/CD automation for building and testing, and comprehensive README documentation covering architecture, functionality, configuration, and deployment.

The intended result is a standalone, Redis-compatible in-memory storage service that can be built, deployed, and accessed by external applications through a standard network interface.
