# Project Plan: LAN-Share

This document outlines the development plan for LAN-Share, a peer-to-peer file transfer utility for local networks. It is designed to guide your team through environment setup, architectural design, and implementation phases.

## 1. Development Environment Setup

First, let's get your Arch Linux system ready for C++20 development with CMake and NeoVim. Open your terminal and run the following commands.

### Install Essential Tools

You'll need a compiler, build tools, and Git for version control. The `base-devel` group in Arch Linux conveniently includes `gcc` (the GNU Compiler Collection), `make`, and other necessary utilities.

Update your system's package database
```
sudo pacman -Syu
```

Install compilers, cmake, git, and neovim
```
sudo pacman -S base-devel cmake git neovim
```

This single command installs:

- g++: The C++ compiler.
- CMake: Your project's build system generator.
- Git: For version control. It's a good practice to use this from day one.
- NeoVim: Your code editor.

### Verify Your Installation

Check that the main components are installed correctly:

```
g++ --version
cmake --version
git --version
nvim --version
```

You should see version information for each tool. Ensure your g++ version supports C++20.

## 2. Understanding the Core Concepts

Before we design the architecture, let's clarify your questions.

### How does FTP work?

Traditional FTP (File Transfer Protocol) is a bit different from your peer-to-peer goal. It's a client-server protocol that uses two separate TCP connections:

1. Control Connection: A persistent connection on port 21 where the client sends commands (e.g., LIST files, RETR to retrieve a file, STOR to store a file) and the server responds.
2. Data Connection: A temporary connection created for the actual file transfer. The server opens this connection to the client (Active Mode) or the client opens it to the server (Passive Mode).

For your project, implementing a full, compliant FTP server would be overly complex and doesn't quite fit the direct peer-to-peer model you've described. A much better and more educational approach is to create your own custom protocol over a single TCP socket, which is what the architecture below will be based on.

### How can I implement file transfer?

You'll use TCP sockets. A TCP socket provides a reliable, ordered, and error-checked stream of data between two computers. The basic process will be:

1. The "receiver" application starts and listens for incoming connections on a specific port (e.g., port 50000).
2. The "sender" application connects to the receiver's IP address and port.
3. Once connected, you define a simple protocol. For example, the sender could first send a small "header" containing metadata like the filename and file size.
4. The receiver reads this header and prepares to receive the file.
5. The sender then reads the file from the disk in chunks (e.g., 4KB at a time) and sends each chunk over the socket.
6. The receiver reads these chunks from the socket and writes them to a new file on its disk until the entire file is received.

### How can I optimize for speed in C++?

Speed comes from efficient I/O and processing. Modern C++ helps a lot here:

- Asynchronous Operations: This is the most significant optimization. Instead of your program "blocking" (waiting) while reading from a file or the network, you can use asynchronous operations to do other work. The Boost.Asio library is the de-facto standard for C++ networking and is a perfect tool for this. It will allow you to handle network events efficiently without juggling threads manually.
- Large Buffers: When reading from the disk and writing to the network (and vice-versa), using a reasonably large buffer (e.g., 4KB to 64KB) is more efficient than sending data byte-by-byte, as it reduces the number of system calls.
- `std::thread`: For your extended features, like running a TUI, a web server, and network discovery simultaneously, you'll need concurrency. `std::thread` is the standard C++ way to manage different threads of execution.

What are some good practices to consider?

- Version Control Everything: Create a Git repository on GitHub or GitLab before you write a single line of code. Commit small, logical changes often.
- Modular Design: Keep different parts of your code separate (networking, UI, file handling). The architecture below will guide you on this. This makes the code easier to test, debug, and extend.
- RAII (Resource Acquisition Is Initialization): Use smart pointers like `std::unique_ptr` and `std::shared_ptr` to manage memory and other resources (like file handles or network sockets). This is a cornerstone of modern C++ that helps prevent resource leaks.
- Use `std::filesystem`: Since you're using C++20, the `<filesystem>` header is your best friend for handling file paths, checking if files exist, and getting file sizes in a cross-platform way.

## 3. Software Requirements

### Functional Requirements (FR)

- FR1: The application shall function as a command-line tool.
- FR2: The application shall be able to send one or more files to a recipient on the same LAN by specifying the recipient's IPv4 address.
- FR3: The application shall be able to receive files from a sender on the same LAN.
- FR4: The file transfer process must support all file formats without corruption.
- FR5: The application shall host an embedded web server on a local port.
- FR6: Users on the same LAN shall be able to connect to the web server via a browser to upload files to the machine running the application.

### Extended Features (Future)

- EF1: The application shall be able to scan the LAN to discover other instances of the same application that are ready to receive files.
- EF2: The application shall provide a Terminal User Interface (TUI) for a more interactive experience.
- EF3: The application shall support optional file compression before sending and decompression after receiving.
- EF4: The application shall support optional encryption of file data during transfer.

### Non-Functional Requirements (NFR)

- NFR1: The application must be built with C++20 or newer.
- NFR2: The primary build tool shall be CMake.
- NFR3: The application must target Linux and Windows platforms.
- NFR4: The file transfer shall be optimized for high-speed data transfer within the constraints of the network.

## 4. Proposed Software Architecture

A modular, layered architecture will be best. This allows you to build and test each part independently and easily add features later.

```
+-------------------------------------------------------------+
|                       User Interfaces                       |
| +-----------------+  +-----------------+  +---------------+  |
| |  Terminal CLI   |  |  TUI (Future)   |  |  Web Server   |  |
| |   (Module A)    |  |   (Module B)    |  |  (Module C)   |  |
| +-----------------+  +-----------------+  +---------------+  |
+-------------------------------------------------------------+
| (Communicates via API)
+-------------------------------------------------------------+
|                        Core Logic                           |
| +---------------------------------------------------------+ |
| |                    Transfer Manager                     | |
| |  (Handles sessions, file metadata, progress tracking)   | |
| +---------------------------------------------------------+ |
|      |                  |                 |                 |
| +----------+   +-------------------+   +-------------+   +-----------+
| | File I/O |   | Network Protocol  |   |  Discovery  |   |Compressor |
| | (fs)     |   | (Header/Payload)  |   |  (Future)   |   | (Future)  |
| +----------+   +-------------------+   +-------------+   +-----------+
+-------------------------------------------------------------+
| (Uses)
+-------------------------------------------------------------+
|                     Low-Level Services                      |
| +---------------------------------------------------------+ |
| |                  Networking (Boost.Asio)                | |
| +---------------------------------------------------------+ |
+-------------------------------------------------------------+
```

### Module Descriptions

- Networking (Low-Level Service): This is the foundation. It will be a wrapper around a library like Boost.Asio. Its only job is to connect, disconnect, send raw bytes, and receive raw bytes asynchronously.
- Core Logic: This is the brain of your application.
- Network Protocol: Defines the "language" for your transfer (e.g., send header with filename/size, then send file data).
- File I/O: Uses `std::filesystem` to read and write files from/to the disk.
- Transfer Manager: Orchestrates the entire process. A UI module will say "send this file to this IP", and the manager will use the other core components to make it happen.
- User Interfaces (UI): These are the ways a user interacts with your application. They are completely separate from the core logic.
- Terminal CLI (Module A): Your first goal. A simple command-line interface that takes arguments like `lanshare send <file> <ip_address>` or `lanshare receive`.

- Web Server (Module C): A module that uses a C++ HTTP library (like `cpp-httplib` for simplicity) to listen for HTTP requests. When a file is uploaded, it passes the data to the Core Logic.

## 5. Project Roadmap & Task Breakdown

### Phase 1: Core Command-Line Tool (FR1-FR4)

Goal: Send and receive a file between two terminals on the same network.

- Task 1.1: Project Setup
    - Create a Git repository.
    - Set up a CMakeLists.txt file for a C++20 executable.
    - Write a "Hello, World!" to ensure your toolchain works.

- Task 1.2: Networking Foundation

- Choose and set up Boost.Asio.

- Create a simple TCP "echo" server and client. The client sends a message, and the server sends it back. This proves your basic networking is solid.

- Task 1.3: File Sending Logic
    - In your "client" code, add the ability to open a file.
    - Define your transfer protocol (e.g., a simple header). Start with just the filename and file size.
    - Implement the logic:
        1. Connect to the receiver.
        2. Send the header.
        3.  Read the file in chunks and send each chunk over the network.

- Task 1.4: File Receiving Logic
    - In your "server" code, implement the logic:
        1. Listen for and accept a connection.
        2. Read the header to get the filename and size.
        3. Create a new file with that name.
        4. Read data from the network in chunks and write it to the new file until the expected number of bytes has been received.
- Task 1.5: Build the CLI
    - Integrate the sender and receiver logic into a single executable.
    - Use command-line arguments (e.g., using a simple argv/argc check) to decide whether to run in send or receive mode and to get the target IP/filename.

### Phase 2: The Web Server Interface (FR5-FR6)
Goal: Allow users to upload files through a web browser.

- Task 2.1: Integrate an HTTP Library
    - Research and choose a simple, header-only C++ HTTP library like cpp-httplib.
    - Integrate it into your CMake project.
- Task 2.2: Create the Web Module
    - Create a WebServer class that runs in its own thread (std::thread).
    - Set up a handler for the root URL (/) to serve a basic HTML page.
    - Set up a handler for POST requests to an /upload endpoint.
- Task 2.3: Build the Web Page
    - Create a simple index.html file with a form that allows a user to select a file and has an "Upload" button. This form should POST to your /upload endpoint.
- Task 2.4: Connect Web to Core
    - When the /upload handler in your C++ code receives a file, it should not save the file directly. Instead, it should pass the file data and name to your Core Logic's Transfer Manager, just as if it were a local operation. This reuses your existing logic.

### Phase 3: Extended Features (EF1-EF4)
Goal: Add advanced functionality.

- Task 3.1: Network Discovery (EF1)
    - Research UDP Broadcast or Multicast. UDP is a connectionless protocol perfect for shouting "is anyone out there?" to the whole network.
    - Implement a discovery mechanism where a receiving client listens on a UDP port, and a sending client can broadcast a message to find active receivers.
- Task 3.2: TUI (EF2)
    - Research a TUI library like FTXUI or ncurses.
    - Create a new UI module that provides a more interactive, visual experience in the terminal.