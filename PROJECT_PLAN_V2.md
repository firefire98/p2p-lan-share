

# **Development Plan for a Cross-Platform Peer-to-Peer File Transfer Application**

## **1\. Introduction**

### **1.1. Project Goal**

The primary objective of this project is to develop a robust peer-to-peer (P2P) file transfer software application. This application will be engineered using C++20, leveraging CMake as its build system. It is designed for cross-platform compatibility, initially targeting Arch Linux, general Linux distributions, and Windows. The core functionality will enable users to transfer files directly between peers on a Local Area Network (LAN) by specifying the target peer's IP address. To enhance usability, the software will feature both a Command-Line Interface (CLI) for traditional terminal-based operation and a web-based interface accessible via a browser for more intuitive interaction. The project roadmap also includes extended features such as an interactive Text User Interface (TUI), data security through encryption, and file compression/decompression capabilities.

### **1.2. Report Purpose**

This report serves as a comprehensive development plan for the P2P file transfer software. It aims to provide a clear and detailed blueprint covering all critical aspects of the project lifecycle. This includes refined project requirements, a proposed software architecture, an evaluation of technology choices and libraries, a detailed task breakdown into manageable project stages, and an outline for future enhancements. The plan is intended to guide the development process, ensuring a structured approach towards achieving the project's goals.

### **1.3. Scope**

The scope of this document encompasses the initial development phases required to deliver a functional P2P file transfer application with both CLI and web interfaces. It details the design and implementation strategies for the core transfer mechanism, user interaction layers, and the cross-platform build system. Additionally, this report outlines a roadmap for subsequent development efforts that will introduce advanced features, thereby extending the software's capabilities and utility.

## **2\. Refined Project Requirements**

### **2.1. Core Functionality**

The foundational elements of the software are defined as follows:

* **Peer-to-Peer File Transfer:** The software must enable direct file exchange between two connected computer systems (peers) without reliance on a central server.  
* **LAN-Based Operation:** File transfers will occur within a Local Area Network, with peers identifying each other via direct IP address and port connections.  
* **Target Platforms:** The primary development environment will be Arch Linux. The software must also be fully functional on general Linux distributions and Microsoft Windows operating systems.  
* **Programming Language:** The application will be developed using C++20, taking advantage of its modern features and standard library enhancements.  
* **Build Tool:** CMake will be utilized as the build system generator, facilitating cross-platform compilation and dependency management.

### **2.2. User Interface**

To cater to different user preferences and operational contexts, the software will provide two distinct interfaces:

* **Command-Line Interface (CLI):** A functional CLI will allow users to initiate file sending and receiving operations, specify target peers, and monitor transfer progress directly from a terminal.  
* **Web Interface:** An embedded web server will host a user-friendly interface accessible via a standard web browser on the LAN. This interface will provide an alternative means for selecting files to share, initiating transfers to other peers, and viewing transfer status.

### **2.3. File Transfer Protocol**

A critical decision is the choice of protocol for file transfer. After comparing FTP (File Transfer Protocol) and a custom solution over raw TCP/IP, the latter is deemed more appropriate.

FTP is an application-layer protocol that itself operates over TCP/IP.1 While it offers a rich set of commands for file operations like renaming and deletion 3, this extensive feature set introduces complexity that is largely unnecessary for the core P2P file sending and receiving task. Standard FTP also presents significant security vulnerabilities, as it transmits user credentials and file data in plain text.1 Although FTPS (FTP Secure) addresses these security concerns by adding SSL/TLS encryption, it introduces further complexities in terms of certificate management and firewall traversal, often due to its use of multiple ports for control and data channels.2 The dual-channel architecture of FTP (a control channel and a separate data channel for each transfer) can also lead to increased overhead and potential inefficiencies, particularly when dealing with numerous small files or rapid transfers.3

Conversely, building a custom application-level protocol directly on TCP/IP provides greater control and flexibility. It allows the protocol to be tailored specifically to the needs of this P2P application, implementing only the necessary messaging for file metadata exchange, transfer initiation, data chunking, and acknowledgments. This approach can result in a more lightweight and efficient solution compared to a full FTP implementation. Security can be robustly implemented by layering TLS directly onto the TCP connection, effectively securing the custom protocol without the inherent complexities of FTPS. This decision to use raw TCP/IP simplifies the initial development by avoiding the baggage of FTP's comprehensive command set and its associated overhead, while still providing a solid foundation for secure and reliable data exchange. It does, however, necessitate the careful design and implementation of this custom application-level protocol.

### **2.4. Extended Features (Roadmap)**

The following features are planned for future development cycles to enhance the software:

* **Text User Interface (TUI):** An improved, interactive terminal interface offering a richer user experience than the basic CLI.  
* **Security:** Encryption of all data in transit using Transport Layer Security (TLS) or a similar robust encryption mechanism. This will also include peer authentication.  
* **Compression/Decompression:** Functionality to compress files before transfer and decompress them upon receipt, reducing transfer times and bandwidth consumption.  
* **LAN Peer Discovery:** An automated mechanism for peers on the same LAN to discover each other without requiring manual IP address entry, significantly improving usability.

The requirement for dual interfaces (CLI and Web) naturally leads to the need for a modular software architecture. The core P2P logic must be effectively decoupled from these presentation layers. This implies that the central engine of the application should expose a well-defined set of Application Programming Interfaces (APIs) that both the CLI and the web server backend can consume. Such separation is fundamental for maintaining a clean codebase, facilitating independent development and testing of UI components, and allowing for future changes or additions to interfaces without impacting the core transfer mechanism.

## **3\. Proposed Software Architecture**

### **3.1. Overview**

A modular, layered software architecture is proposed. This design philosophy emphasizes separation of concerns, which is paramount for achieving testability, maintainability, and scalability, especially given the requirements for cross-platform support and multiple user interfaces.

### **3.2. Core P2P Engine (CPE)**

The CPE is the heart of the application, responsible for all fundamental P2P operations.

* **Responsibilities:** The CPE will manage all aspects of peer-to-peer connections, implement the custom TCP/IP-based file transfer protocol, handle low-level file input/output (I/O) operations, and maintain the state of ongoing transfers.  
* **Components:**  
  * **Networking Module:** This module will encapsulate all TCP socket communication logic, including listening for incoming connections, initiating outgoing connections to peers, and the reliable sending and receiving of data packets. It will leverage a library like Asio for cross-platform network programming.  
  * **File Transfer Protocol Module:** This component will implement the application-level logic for the custom file transfer protocol. Its responsibilities include exchanging file metadata (such as filename and size), managing the chunking of files for efficient transmission, sequencing data packets, and handling acknowledgments or control messages between peers.  
  * **File I/O Module:** Responsible for interacting with the local filesystem. This includes reading file data from the disk when sending and writing received file data to the disk on the recipient's end.  
  * **Transfer Management Module:** This module will track the status of all active and pending file transfers, monitor their progress, manage transfer queues (if applicable), and handle any errors or exceptions that occur during the transfer process.

### **3.3. Interface Layers**

These layers provide the means for users to interact with the CPE.

* **Command-Line Interface (CLI) Module:**  
  * **Responsibilities:** This module will parse commands entered by the user in a terminal environment. It will interact with the CPE's exposed APIs to initiate actions such as sending a file to a specified peer or preparing to receive a file. It will also display transfer progress, status messages, and any errors to the console.  
* **Web Interface Module:**  
  * **Responsibilities:** This module will host an embedded HTTP server to provide a web-based user interface. It will serve static web assets (HTML, CSS, JavaScript) to the user's browser and handle API requests originating from this UI. These API requests will be translated into calls to the CPE's functions.  
  * **Components:**  
    * **HTTP Server:** A lightweight, embedded HTTP server (e.g., cpp-httplib) will listen for incoming HTTP requests from web browsers on the LAN.  
    * **API Handler:** This component will define and process the RESTful API endpoints. It will validate incoming requests, interact with the CPE to perform actions like listing shareable files or initiating a transfer, and formulate HTTP responses.  
    * **Web UI Assets:** Static files (HTML for structure, CSS for styling, JavaScript for client-side interactivity) that constitute the visual web interface.

### **3.4. Cross-Platform Abstraction Layer (CPAL)**

* **Responsibilities:** The CPAL is designed to encapsulate any platform-specific code that cannot be uniformly handled by the C++20 standard library, std::filesystem, or the chosen third-party cross-platform libraries (like Asio). This might include, for example, very specific network interface queries or nuanced filesystem interactions that differ significantly between Windows and Linux and are not abstracted by existing tools.  
* **Goal:** The primary goal of the CPAL is to minimize the amount of platform-dependent \#ifdef directives or conditional code paths within the CPE and the main interface layers. This promotes cleaner, more portable high-level code. While C++20 and libraries like Asio provide excellent cross-platform support, the CPAL acts as a strategic component for managing any unforeseen platform-specific requirements that may arise, particularly in areas like advanced network configuration or system integration. By isolating such code, the bulk of the application remains platform-agnostic, and supporting a new platform in the future might primarily involve implementing its specific version of the CPAL.

### **3.5. Data Flow (Example: Sending a file via CLI)**

The interaction between modules can be illustrated with an example:

1. The user issues a command like send \<filepath\> \<ip\_address\> in the terminal.  
2. The CLI Module parses this command and validates the provided filepath and IP address.  
3. The CLI Module then invokes an appropriate API function within the Core P2P Engine, requesting to send the specified file to the target IP address.  
4. The CPE's Networking Module attempts to establish a TCP connection to the peer at the given IP address and port.  
5. Upon successful connection, the CPE's File Transfer Protocol Module sends file metadata (e.g., filename, size, hash) to the receiving peer.  
6. The CPE's File I/O Module begins reading the source file from the disk in manageable chunks.  
7. The CPE's Networking Module transmits these file chunks sequentially over the established TCP connection.  
8. Throughout this process, the CPE's Transfer Management Module updates the progress of the transfer. The CLI Module can periodically query this status from the CPE and display it to the user.  
9. Once all chunks are sent and the receiving peer acknowledges successful receipt, the connection is closed by the Networking Module.

This modular design, with clear separation between the CPE, CLI Module, Web Module, and CPAL, is fundamental to managing the project's complexity. It allows different segments of the application to be developed, tested, and maintained more independently. For instance, the Web UI could undergo a complete redesign without necessitating changes to the core file transfer logic within the CPE, provided the API contract between the web backend and the CPE remains stable. This decoupling, where the CLI is unaware of TCP socket intricacies and the CPE is agnostic to command-line argument parsing, is a characteristic of a well-structured layered architecture, leading to a more robust and understandable system.

## **4\. Technology Stack and Libraries**

The selection of appropriate technologies and libraries is crucial for efficient development, cross-platform compatibility, and achieving the desired performance and features.

### **4.1. Core Networking: Asio (Standalone or Boost.Asio)**

* **Rationale:** Asio is a mature, extensively used, cross-platform C++ library specifically designed for network and low-level I/O programming. It provides developers with a consistent asynchronous programming model using modern C++ idioms.5 Its high-level abstractions over raw operating system sockets (like Berkeley sockets or Winsock) significantly simplify the development of TCP client and server applications. Furthermore, Asio's design integrates well with modern C++20 features, including coroutines, which can lead to cleaner and more manageable asynchronous code compared to traditional callback-based approaches. While raw OS sockets could be used 6, Asio is preferred for its superior abstraction, portability, and features that reduce boilerplate and complexity.  
* **Integration:** Asio can be used as a header-only library or compiled as a separate library. For this project, integrating it via CMake's FetchContent module is recommended, allowing the build system to download and prepare a specific version of Asio automatically.5 This approach ensures consistency and reproducibility across development environments. The choice of Asio is pivotal as its asynchronous, event-driven model is well-suited for efficient I/O operations in network applications, steering development away from potentially error-prone manual thread management for individual connections.

### **4.2. Web Interface Backend: C++ HTTP Server Library**

* **Requirements:** The embedded HTTP server for the web interface needs to be lightweight, easily embeddable within the C++ application, straightforward to integrate using CMake, and compatible with C++17/20.  
* **Options and Recommendation:**  
  * **cpp-httplib:** This library is a strong candidate due to its simplicity and ease of use. It can be included as a single header file (or multiple files if preferred) and offers essential features like SSL support (for HTTPS), static file serving, and handling of multipart/form-data, which would be useful for file uploads through the web interface if that feature is added.8 Its minimal dependencies and straightforward API make it ideal for rapid development of the required web UI backend.  
  * **Crow:** A micro-framework for C++ that provides more advanced features like sophisticated routing, middleware support, and built-in JSON handling.10 It typically requires Boost. While powerful, its added complexity might be overkill for the initial scope of the web interface.  
  * **libhttpserver:** This library is built upon libmicrohttpd and is specifically designed for creating RESTful web services.12 It might be considered if the web API becomes very complex.  
  * **Boost.Beast:** A lower-level library that builds on Asio, offering high performance and flexibility for HTTP and WebSocket programming.13 However, it has a steeper learning curve and might be more involved than necessary for this project's web UI.

    For the initial implementation, cpp-httplib is recommended due to its balance of features and simplicity, minimizing external dependencies and build complexities.

### **4.3. Command-Line Interface (CLI)**

* **Initial Implementation:** Standard C++ iostreams (std::cout for output, std::cin for input) will be sufficient for the basic CLI functionality.  
* **Argument Parsing (Optional but Recommended):** As CLI commands and options grow in complexity, a dedicated argument parsing library can be beneficial.  
  * cxxopts: A popular, header-only C++ library for parsing command-line options.  
  * Boost.Program\_options: A more comprehensive and powerful library, part of the Boost collection.  
    The recommendation is to start with iostreams and evaluate the need for a library like cxxopts if the CLI's argument handling requirements become intricate.

### **4.4. Text User Interface (TUI) (Extended Feature)**

* **Options and Recommendation:** For the extended TUI feature, several modern C++ libraries are available:  
  * **FTXUI:** This library is appealing for its modern C++ design, functional programming style inspiration, lack of external dependencies, and support for interactive elements and animations.14  
  * **TermOx:** A C++20 TUI library that provides a widget-based approach to building terminal interfaces.16  
  * **cpp-terminal:** Aims to be a simpler, lightweight replacement for ncurses, focusing on cross-platform terminal control.17  
  * **ncurses:** The classic C library for creating TUIs, very powerful but with a C-style API that might feel less integrated with modern C++ code.18

    For a modern C++ project, FTXUI or TermOx would be excellent choices, offering a more idiomatic C++ development experience for creating a rich TUI.

### **4.5. Security (Extended Feature): OpenSSL**

* **Rationale:** OpenSSL is the de facto industry standard library for implementing TLS/SSL and other cryptographic functions.20 Both Asio (for TCP stream encryption) and  
  cpp-httplib (for HTTPS) can integrate with OpenSSL to provide secure, encrypted communication channels.5  
* **Usage:** OpenSSL will be used to encrypt the custom P2P TCP communication and to enable HTTPS for the web interface, protecting data from eavesdropping.  
* **Considerations:** Working with OpenSSL's C API requires careful resource management. It is crucial to use RAII (Resource Acquisition Is Initialization) wrappers for OpenSSL objects (like SSL\_CTX, SSL, BIO) to ensure proper cleanup and prevent resource leaks or security vulnerabilities.21

### **4.6. Compression (Extended Feature)**

* **Options and Recommendation:** To reduce file transfer sizes:  
  * **Zstandard (Zstd):** A modern compression algorithm known for its very fast compression and decompression speeds, often at compression ratios comparable to or better than zlib.22 It is well-suited for real-time compression scenarios.  
  * **zlib:** A ubiquitous and highly stable compression library providing the DEFLATE algorithm.24 It offers a good balance of compression ratio and speed and is widely available.

    Zstandard is recommended for potentially better performance, especially if its integration complexity is manageable. zlib remains a solid and reliable alternative.

The selection of these libraries, particularly for extended features, highlights the importance of designing the Core P2P Engine with extensibility in mind. For instance, the file transfer protocol module should be architected so that a compression/decompression layer can be inserted into the data pipeline without requiring a major overhaul. Similarly, the networking module should be adaptable to easily wrap its communications within an SSL/TLS layer provided by OpenSSL and Asio. This foresight in architectural design helps prevent costly refactoring efforts when these advanced features are implemented.

## **5\. Protocol Choice: FTP vs. TCP/IP**

The choice of communication protocol is a fundamental architectural decision that significantly impacts the design, complexity, and performance of the file transfer application.

### **5.1. Understanding TCP/IP**

Transmission Control Protocol (TCP) is a core protocol of the Internet protocol suite. It provides reliable, ordered, and error-checked delivery of a stream of octets (bytes) between applications running on hosts communicating over an IP network.1 TCP is connection-oriented, meaning a connection must be established between two endpoints before data can be exchanged. It segments large data into packets, manages their transmission, and reassembles them at the destination, ensuring data integrity.1 TCP operates in conjunction with the Internet Protocol (IP), which handles the addressing of data packets and their routing across networks.1

### **5.2. Understanding FTP**

File Transfer Protocol (FTP) is an application-layer protocol built on top of TCP/IP, specifically designed for the purpose of transferring computer files between a client and a server on a network.1 A key characteristic of FTP is its use of two separate TCP connections:

1. **Control Channel:** Typically established on server port 21, this connection is used for transmitting commands (e.g., login, list directory, get file) and server responses. This channel remains open for the duration of the FTP session.3  
2. **Data Channel:** A separate TCP connection is established for each actual file transfer or directory listing. The port used for the data channel can vary depending on whether FTP is operating in active or passive mode.3

### **5.3. Comparison for P2P File Transfer**

When evaluating FTP against a custom protocol over raw TCP/IP for this P2P application, several factors come into play:

* **Overhead:**  
  * **FTP:** The establishment of two connections and the exchange of numerous control commands can introduce protocol overhead and latency, particularly when initiating transfers or handling many small files.3 While some sources describe FTP's communication as having low overhead in general 4, the dual-channel nature and the handshake-intensive command structure are inherent.  
  * **TCP/IP (Custom Protocol):** Building directly on TCP allows for a streamlined, custom protocol. Only the essential handshakes and data exchanges for the P2P file transfer are implemented, potentially minimizing overhead compared to the comprehensive FTP standard.  
* **Complexity:**  
  * **FTP:** Implementing a full FTP client and server is a complex undertaking due to its extensive command set (which includes operations like file renaming, deletion, directory navigation, user authentication methods, etc.).3 Many of these features are superfluous for a direct P2P file send/receive mechanism.  
  * **TCP/IP (Custom Protocol):** This approach requires the definition and implementation of a custom application-level protocol. While this involves design effort, the protocol can be tailored to be very simple, focusing solely on the specific needs of the P2P file transfer use case.  
* **Security:**  
  * **Standard FTP:** A major drawback of standard FTP is its inherent insecurity. It transmits user credentials (usernames and passwords) and file data in plain text, making it highly vulnerable to eavesdropping and interception.1  
  * **FTPS (FTP over SSL/TLS):** FTPS enhances FTP by adding a layer of security through SSL/TLS encryption. It can encrypt the control channel, the data channel, or both.2 However, FTPS can be more complex to configure and manage, especially concerning firewalls and the dynamic allocation of ports for data channels.4  
  * **TCP/IP (Custom Protocol):** Raw TCP itself offers no encryption. However, TLS can be layered directly on top of a TCP connection to secure the custom application-level protocol. This provides a level of security comparable to FTPS but potentially with simpler network configuration, as it typically involves a single encrypted port.  
* **Control & Flexibility:**  
  * **FTP:** Adherence to the FTP standard means less flexibility for implementing custom P2P interactions or messages that fall outside the scope of traditional file transfer operations.  
  * **TCP/IP (Custom Protocol):** This approach offers complete control over the communication protocol. It allows for the design of custom messages, peer negotiation strategies, optimized data flow control, and easier extension for future P2P-specific features.

### **5.4. Decision and Rationale**

**Decision:** For the core file transfer mechanism of this P2P application, a custom protocol implemented directly over **raw TCP/IP** is the recommended choice.

**Rationale:**

1. **Simplicity for the Core Task:** The primary requirement is direct file transfer between two peers. A custom protocol over TCP can be designed to be significantly simpler and more efficient for this specific task than implementing or interfacing with a full FTP client/server stack.3  
2. **Reduced Overhead for P2P:** By avoiding the mandatory FTP control channel and its associated command exchanges for simple transfers, connection setup can be faster, and protocol chatter can be minimized.  
3. **Enhanced Control and Tailoring:** A custom protocol provides complete control over the data exchange process, error handling mechanisms, and the potential for future extensions with custom P2P messages not envisioned by the FTP standard.  
4. **Streamlined Security Integration:** TLS can be directly integrated over the TCP sockets (e.g., using Asio's SSL stream capabilities in conjunction with OpenSSL). This allows for robust encryption of the custom protocol, offering security comparable to FTPS but with potentially simpler management than FTPS's dual-channel complexity and associated firewall traversal challenges.2 A single TCP connection, possibly upgraded to TLS, is generally easier to manage from a network configuration perspective.  
5. **Alignment with P2P Paradigm:** Most contemporary P2P applications opt for custom protocols built over TCP or UDP rather than relying on heavyweight, traditional application-layer protocols like FTP.

The choice of raw TCP/IP, while simplifying by avoiding the broad feature set of FTP, shifts the responsibility to the developers to meticulously design and implement a robust custom application-level protocol. This protocol must effectively handle file metadata exchange, data chunking, transfer control, error reporting, and acknowledgments. While this offers greater control, it requires careful engineering to ensure reliability and efficiency. Furthermore, the inherent lack of security in raw TCP means that the planned integration of TLS is not merely an add-on but a critical component for any modern, trustworthy application.

## **6\. Build System and Cross-Platform Strategy (CMake)**

CMake will be employed as the build system generator, providing a robust and flexible framework for managing the compilation process across the target platforms (Arch Linux, general Linux, and Windows) and for handling dependencies.

### **6.1. Project Structure and CMake Configuration**

A well-organized project structure is essential for maintainability and scalability.

* **Recommended Directory Structure:**  
  MyP2PApp/  
  ├── CMakeLists.txt       \# Top-level CMake configuration file  
  ├── build/                 \# Directory for all build artifacts (out-of-source builds)  
  ├── src/                   \# Contains all source files (.cpp)  
  │   ├── core/              \# Logic for the Core P2P Engine  
  │   ├── cli/               \# Logic specific to the Command-Line Interface  
  │   ├── web/               \# Logic specific to the Web Interface backend  
  │   └── main.cpp           \# Main entry point of the application  
  ├── include/               \# Contains all public header files (.h,.hpp)  
  │   └── MyP2PApp/          \# Project-specific headers, organized by module if necessary  
  ├── third\_party/           \# Optional: for vendored dependencies or Git submodules  
  │                          \# (if not exclusively using FetchContent)  
  └── tests/                 \# Directory for unit tests and integration tests

  This structure clearly separates implementation details (src/) from public interfaces (include/), isolates test code, and ensures that build artifacts are generated outside the source tree, keeping the project clean.26  
* **Top-Level CMakeLists.txt Essentials:**  
  * cmake\_minimum\_required(VERSION 3.15): Specifies the minimum CMake version. Using a reasonably modern version (e.g., 3.15 or higher) ensures access to important features like improved FetchContent capabilities and modern target-based commands.27  
  * project(MyP2PApp VERSION 1.0.0 LANGUAGES CXX): Declares the project name, version, and specifies C++ as the programming language.  
  * C++ Standard Configuration:  
    CMake  
    set(CMAKE\_CXX\_STANDARD 20)  
    set(CMAKE\_CXX\_STANDARD\_REQUIRED True)  
    set(CMAKE\_CXX\_EXTENSIONS Off)

    These commands enforce the C++20 standard, make it a requirement, and disable compiler-specific extensions, which promotes better code portability.28  
  * Include Directories: target\_include\_directories(MyP2PApp PUBLIC "${PROJECT\_SOURCE\_DIR}/include") makes headers in the include directory available to the MyP2PApp target and any targets that link against it. Using target\_\* commands is a cornerstone of modern CMake practice.28  
  * Executable Definition: add\_executable(MyP2PApp src/main.cpp src/core/cpe\_module.cpp...) defines the main executable target and lists its source files.  
* **Modular CMakeLists.txt Files:** For larger logical components such as the core engine, cli interface, and web backend, it is advisable to create separate CMakeLists.txt files within their respective subdirectories (e.g., src/core/CMakeLists.txt). These modules can then be built as static or shared libraries using add\_library() and subsequently linked to the main MyP2PApp executable using target\_link\_libraries(). This approach enhances modularity, improves the organization of the build system, and can lead to faster rebuild times during development.28

### **6.2. Handling Platform-Specific Code**

CMake provides mechanisms to manage platform-dependent code and configurations:

* **Conditional Compilation with CMake Variables:** CMake defines variables such as WIN32 (for Windows), UNIX (for Unix-like systems including Linux and macOS), and APPLE (for macOS). These can be used within if() blocks in CMakeLists.txt to conditionally include platform-specific source files, apply platform-dependent compiler flags, or set specific preprocessor definitions.27  
  CMake  
  if(WIN32)  
      \# Windows-specific configurations  
      target\_sources(MyP2PApp PRIVATE platform/windows/win\_network\_utils.cpp)  
      target\_compile\_definitions(MyP2PApp PRIVATE IS\_WINDOWS)  
  elseif(UNIX AND NOT APPLE) \# Specifically for Linux  
      \# Linux-specific configurations  
      target\_sources(MyP2PApp PRIVATE platform/linux/linux\_daemon\_utils.cpp)  
      target\_compile\_definitions(MyP2PApp PRIVATE IS\_LINUX)  
  endif()

* **Abstraction Layers (CPAL):** As detailed in the software architecture (Section 3.4), a Cross-Platform Abstraction Layer should be implemented to encapsulate any platform-specific logic that cannot be uniformly addressed by standard C++ features or the chosen cross-platform third-party libraries.  
* **std::filesystem:** The C++17 \<filesystem\> library should be the primary tool for path manipulation, file system queries, and directory operations, as it is designed to be platform-agnostic.  
* **Minimize Platform-Specific Code:** The overarching strategy should be to maximize the use of cross-platform libraries (like Asio) and standard C++ features. This inherently reduces the quantity of code that requires platform-specific handling, leading to a more maintainable and portable codebase.26

### **6.3. Dependency Management with CMake**

CMake offers several ways to manage external dependencies:

* **FetchContent Module:** This is a modern CMake module ideal for downloading dependency sources at configure-time and building them as part of the main project. It is particularly well-suited for header-only libraries or libraries that have good CMake support themselves. FetchContent allows specifying Git repositories and particular tags or commit hashes, which is crucial for ensuring reproducible builds.27 Pinning dependencies to specific versions prevents unexpected build failures that could arise from unmonitored upstream changes in a dependency if, for example, the latest development branch were always pulled.  
  CMake  
  include(FetchContent)  
  FetchContent\_Declare(  
      asio\_dep \# A unique name for this content declaration  
      GIT\_REPOSITORY https://github.com/chriskohlhoff/asio.git  
      GIT\_TAG asio-1\-28\-0 \# Use a specific, stable tag for reproducibility  
  )  
  FetchContent\_MakeAvailable(asio\_dep)  
  \# For header-only Asio, add its include directory to your target:  
  \# target\_include\_directories(MyP2PApp PRIVATE ${asio\_dep\_SOURCE\_DIR}/asio/include)  
  \# If Asio were a compiled library providing a CMake target (e.g., Asio::asio):  
  \# target\_link\_libraries(MyP2PApp PRIVATE Asio::asio)

* **find\_package() Command:** This command is used to locate and use libraries that are already installed on the development system or are provided by a system package manager (e.g., OpenSSL, Zlib on Linux). When using find\_package(), modern CMake practice favors linking against imported targets (e.g., OpenSSL::SSL, OpenSSL::Crypto) if the found package provides them, rather than relying on older-style CMake variables.27  
  CMake  
  find\_package(OpenSSL REQUIRED)  
  target\_link\_libraries(MyP2PApp PRIVATE OpenSSL::SSL OpenSSL::Crypto)

* **CPM.cmake:** This is a popular community-provided CMake script that acts as a wrapper around FetchContent. It aims to simplify dependency management by providing a more concise API, automatic version checking across dependencies, and improved caching mechanisms.31 CPM.cmake can be particularly useful for projects with numerous dependencies. It also offers an option (  
  CPM\_USE\_LOCAL\_PACKAGES) to first attempt find\_package() for a dependency and fall back to fetching and building from source if the system package is not found.31 While powerful, it's important to be aware that, by default, it may rebuild dependencies from scratch in new build directories unless a global source cache (like  
  CPM\_SOURCE\_CACHE environment variable) is configured.  
* **Choosing the Method:**  
  * For libraries not commonly available as pre-installed system packages, or when a very specific version is critical, or for integrating header-only libraries, FetchContent (or CPM.cmake) is generally the preferred approach.  
  * For well-established system libraries (e.g., OpenSSL on Linux, system zlib) where using the version provided by the OS package manager is acceptable and desirable, find\_package() is appropriate.

### **6.4. Build Process**

* **Out-of-Source Builds:** A fundamental best practice in CMake is to perform out-of-source builds. This means that all build artifacts (Makefiles, project files, object files, executables) are generated in a directory separate from the source code tree (e.g., a build/ directory).26 This keeps the source directory clean and allows for multiple build configurations (e.g., Debug, Release) from the same source.  
  Bash  
  \# Create a build directory (if it doesn't exist) and navigate into it  
  mkdir \-p build  
  cd build

  \# Configure the project (generate build files)  
  \# The \-G option specifies the generator for the native build system  
  \# Examples:  
  \# cmake.. \-G "Ninja"                     \# For Ninja build system  
  \# cmake.. \-G "Unix Makefiles"            \# For Makefiles on Linux/macOS  
  \# cmake.. \-G "Visual Studio 17 2022"   \# For Visual Studio 2022 on Windows  
  cmake.. \-G "Ninja" \# Using Ninja as an example

  \# Compile the project  
  cmake \--build.

  The ability of CMake to generate build files for various native build systems and IDEs using the \-G option is a core strength.27 It allows developers on different platforms to use their preferred tools (e.g., Makefiles or Ninja on Linux, Visual Studio on Windows, Xcode on macOS) while working from the same  
  CMakeLists.txt project definition.  
* **Cross-Compilation (Advanced):** For scenarios such as building Windows executables from a Linux development environment (or vice-versa), CMake supports cross-compilation through the use of toolchain files. A toolchain file (-DCMAKE\_TOOLCHAIN\_FILE=path/to/toolchain.cmake) provides CMake with information about the cross-compilers, linkers, and other tools necessary for the target platform.27 This is an advanced use case and may not be required if native builds are performed on each target platform.

Adopting "Modern CMake" practices—which emphasize defining properties on targets (e.g., target\_link\_libraries, target\_include\_directories, target\_compile\_definitions with PUBLIC/PRIVATE/INTERFACE scope) rather than using older directory-level commands—is crucial for creating a build system that is maintainable, scalable, and truly cross-platform.29 This approach makes dependencies explicit and the overall build graph much easier to understand and debug. Finally, meticulous attention must be paid to file system case sensitivity differences between platforms like Windows (typically case-insensitive) and Linux (case-sensitive). Inconsistent casing in

CMakeLists.txt file references or in \#include directives can lead to builds succeeding on one platform but failing on another, a common pitfall in cross-platform development.26

## **7\. Task Breakdown and Initial Project Stages**

The development of the P2P file transfer software will be structured into distinct stages, each with specific objectives, tasks, and deliverables. This phased approach allows for iterative development, early validation of critical components, and the flexibility to adapt to challenges or new insights as the project progresses.

### **Stage 1: Core P2P Engine & Basic CLI (Proof of Concept)**

* **Objective:** The primary goal of this initial stage is to establish the fundamental peer-to-peer file transfer capability using the chosen raw TCP/IP protocol. A minimal Command-Line Interface (CLI) will be developed for interaction and testing. This stage is critical for validating the core architectural decisions, particularly the efficacy of Asio for networking and the viability of the custom application-level protocol.  
* **Tasks:**  
  1. **T1.1: Project Setup & CMake Configuration:** Establish the project directory structure as defined in Section 6.1. Implement the initial top-level CMakeLists.txt file, configuring it for C++20 and ensuring it can generate build files for both Linux and Windows. Integrate the Asio library, preferably using CMake's FetchContent module to ensure a consistent and specific version is used across all development environments.  
  2. **T1.2: Basic TCP Networking Layer (Core Engine):** Develop the foundational networking logic within the Core P2P Engine. This includes implementing peer connection capabilities, allowing one peer to act as a "server" (listening on a specified IP address and port) and another as a "client" (initiating a connection to the server peer). Asio will be utilized to abstract the complexities of underlying socket operations.5  
  3. **T1.3: Application-Level Protocol Definition & Implementation (Core Engine):** Design and implement a simple, custom application-level protocol that will operate on top of the TCP connections. This protocol must manage:  
     * The exchange of essential file metadata (e.g., filename, total size) between the sender and receiver before transfer begins.  
     * A mechanism for transferring the file data in manageable, sequential chunks.  
     * A system for the receiver to acknowledge received chunks or the successful completion of the entire file transfer.  
     * Basic status messages or error codes for communication issues.  
       This custom protocol is a necessity because raw TCP only provides a reliable byte stream; the interpretation and structure of the data transmitted over that stream must be defined by the application.1  
  4. **T1.4: File I/O Operations (Core Engine):** Implement functions within the Core Engine to efficiently read a local file from disk when a peer is sending, and to write received data chunks to a local file on the recipient peer's system.  
  5. **T1.5: Initial CLI Implementation (CLI Module):** Develop a basic command-line interface using standard C++ iostreams (std::cin, std::cout). This CLI should, at a minimum, support commands such as:  
     * send \<filepath\> \<destination\_ip\>\[:port\]  
     * receive \<listen\_port\> \<save\_directory\> (or a default listening mode if no port is specified).  
       A simple progress indicator (e.g., percentage of file transferred) should also be implemented.  
  6. **T1.6: Integration and Basic Testing:** Integrate the CLI module with the Core P2P Engine, ensuring that CLI commands correctly invoke the engine's functionalities. Conduct initial tests of file transfer between two instances of the application running on the Local Area Network (LAN). Begin with Linux-to-Linux transfers to validate the core logic, then proceed to test Linux-to-Windows (or vice-versa) transfers to identify and address any early cross-platform compatibility issues. Debug and refine the basic transfer process based on test results.  
* **Deliverables:** A working proof-of-concept (PoC) application capable of transferring files between two peers on a LAN using the CLI. A functional CMake build system that successfully compiles the application on both Linux and Windows.  
* **Estimated Duration:** Approximately 4-6 weeks.

### **Stage 2: Web Interface Integration**

* **Objective:** To implement a basic web-based user interface, accessible via a standard web browser on the LAN. This interface will provide an alternative, more graphical way for users to manage shared files and initiate transfers, thereby enhancing the software's overall usability.  
* **Tasks:**  
  1. **T2.1: Select and Integrate HTTP Server Library:** Choose a suitable C++ HTTP server library (e.g., cpp-httplib is recommended for its simplicity and ease of integration 8). Integrate this library into the project using the CMake build system. Configure the basic HTTP server to listen on a designated network port.  
  2. **T2.2: Develop Web API Endpoints (Web Module & Core Engine):** Design and implement a set of RESTful API endpoints that the web frontend will use to communicate with the application's backend. These endpoints will serve as the bridge between the web UI and the Core P2P Engine. Example endpoints include:  
     * POST /api/files/share: Allows the user to select a local file and make it available for sharing with other peers.  
     * GET /api/files/shared: Retrieves a list of files currently marked as shareable by the local peer.  
     * POST /api/transfer/initiate: Triggers the sending of a specified shared file to a given destination IP address and filename.  
     * GET /api/transfer/status/\<transfer\_id\>: Queries the progress and status of an ongoing file transfer.  
  3. **T2.3: Basic Web Frontend Development (Web Module):** Create simple HTML, CSS, and JavaScript pages that will constitute the user interface. This frontend will:  
     * Display a list of locally shared files, perhaps with options to unshare them.  
     * Provide a form or mechanism for selecting a local file to be shared.  
     * Allow the user to input a destination IP address and select one of the shared files to initiate a transfer.  
     * Display the status and progress of active file transfers, updating dynamically via calls to the status API.  
       These static web assets (HTML, CSS, JS files) will be served directly by the integrated C++ HTTP server.  
  4. **T2.4: Integration and Testing:** Integrate the web frontend with the backend API endpoints. Conduct thorough testing of all web UI functionalities using common web browsers (e.g., Chrome, Firefox, Edge) on both Linux and Windows platforms. Ensure that transfers initiated via the web interface are correctly processed by the Core P2P Engine and that the existing CLI functionality remains unaffected and operational.  
* **Deliverables:** A version of the software where users can effectively manage shared files and initiate file transfers to other peers on the LAN using a web browser, in addition to the established CLI functionality.  
* **Estimated Duration:** Approximately 4-6 weeks.

### **Stage 3: Cross-Platform Refinement & Robustness**

* **Objective:** To ensure that the software operates stably, consistently, and reliably across all designated target platforms (Arch Linux, general Linux distributions, and Windows). This stage also focuses on improving error handling mechanisms and logging capabilities.  
* **Tasks:**  
  1. **T3.1: Comprehensive Windows Testing & CMake Refinement:** Conduct exhaustive testing of all application features (CLI, Web UI, Core P2P transfer logic) specifically on the Windows platform. Identify and meticulously resolve any Windows-specific bugs, performance bottlenecks, or build system issues. Refine the CMake scripts as necessary to ensure robust and optimized builds for Windows, paying particular attention to MSVC compiler flags, library linking, and any platform-specific API usage.  
  2. **T3.2: General Linux Distribution Testing:** Test the application on at least one other common Linux distribution (e.g., Ubuntu LTS) to verify broader compatibility beyond the primary Arch Linux development environment. Address any discovered compatibility issues, dependency differences, or behavioral inconsistencies.  
  3. **T3.3: Enhanced Error Handling & Logging:** Implement more comprehensive and user-friendly error handling throughout the Core P2P Engine and the interface layers. This includes gracefully managing a wider range of potential issues, such as network connection drops during transfer, file access errors (e.g., permissions denied, file not found), insufficient disk space on the recipient's side, and invalid user inputs. Introduce a basic but effective logging mechanism (e.g., outputting diagnostic information to the console and optionally to a log file) to aid in troubleshooting and debugging.  
  4. **T3.4: Code Refactoring and Cleanup:** Based on the experience and insights gained during the previous development stages, refactor the existing codebase to improve its clarity, efficiency, and long-term maintainability. Ensure adherence to a consistent coding style across all modules. Improve inline code documentation (comments) to make the code easier to understand for current and future developers.  
* **Deliverables:** A stable and well-tested version of the software characterized by improved reliability, robust error handling, and consistent behavior across all supported target platforms.  
* **Estimated Duration:** Approximately 3-4 weeks.

### **Stage 4: Documentation and Initial Packaging**

* **Objective:** To create essential user and developer documentation and to prepare basic distributable packages for the software, making it easier for end-users to install and use.  
* **Tasks:**  
  1. **T4.1: User Documentation:** Prepare clear, concise, and comprehensive documentation for end-users. This documentation should cover:  
     * Step-by-step installation and setup instructions for both Linux and Windows platforms.  
     * Detailed usage instructions for all features of the Command-Line Interface.  
     * A user-friendly guide to navigating and using the Web Interface.  
     * A section for troubleshooting common issues and frequently asked questions.  
  2. **T4.2: Developer Documentation:** Create documentation aimed at developers who might work on, maintain, or extend the software in the future. This should include:  
     * A high-level overview of the software architecture, detailing the Core P2P Engine, interface layers, and their interactions.  
     * Instructions on how to set up the development environment and build the project from source using CMake.  
     * Key API documentation for the Core P2P Engine, explaining how to interact with its main functionalities.  
     * Guidelines for contributing to the project, including coding standards and testing procedures.  
  3. **T4.3: Basic Packaging:** Prepare simple distributable packages for Linux and Windows. Initially, this could involve creating compressed archives (e.g., .zip files for Windows, .tar.gz archives for Linux) containing the compiled executables, any necessary runtime dependencies (if not statically linked), and the user documentation. For more sophisticated installers, CMake's CPack module can be explored in the future to create platform-native installers (e.g., MSI for Windows, DEB/RPM for Linux).28  
* **Deliverables:** Comprehensive user and developer documentation. Basic packaged versions of the software suitable for distribution on Linux and Windows.  
* **Estimated Duration:** Approximately 2-3 weeks.

This phased development strategy, particularly the emphasis on an early Proof of Concept in Stage 1, is a deliberate choice to address the most significant technical risks—those associated with P2P networking and cross-platform compatibility—at the earliest possible point. By validating the core transfer logic before extensive effort is invested in UI layers, the project can adapt more readily to any fundamental challenges discovered. This iterative approach is a hallmark of agile development methodologies.

The distinct separation of backend API development (T2.2) and frontend web development (T2.3) during Stage 2 allows for a cleaner division of labor and concerns. This modularity means these components can be developed and tested with a degree of independence, a common and effective practice in modern web application development.

While comprehensive, dedicated cross-platform testing is formally scheduled for Stage 3, it is implicitly understood that continuous or at least frequent building and testing on both Linux and Windows environments should occur from Stage 1 onwards. Discovering platform-specific issues early in the development cycle is far less costly to resolve than if they are found much later. The initial CMake configuration established in T1.1 should, by design, support building for both primary target environments from the project's inception.

A high-level summary of this phased plan, including key tasks, deliverables, and estimated durations, is presented in Table 1\.

**Table 1: High-Level Task Breakdown per Stage**

| Stage | Key Tasks | Primary Deliverables | Estimated Duration |
| :---- | :---- | :---- | :---- |
| **Stage 1: Core P2P Engine & Basic CLI (PoC)** | Project setup (CMake, C++20), TCP networking layer (Asio), basic application-level protocol design & implementation, file I/O operations, initial CLI development, integration & basic LAN testing. | Working Proof-of-Concept for CLI-based LAN file transfer; functional CMake build system for Linux and Windows. | 4-6 weeks |
| **Stage 2: Web Interface Integration** | HTTP server library integration (e.g., cpp-httplib), Web API endpoint development (interaction with Core Engine), basic web frontend development (HTML, CSS, JS), integration & cross-browser testing on LAN. | Software version with a functional web UI for file sharing & transfer initiation on the LAN, alongside CLI. | 4-6 weeks |
| **Stage 3: Cross-Platform Refinement & Robustness** | Comprehensive Windows & general Linux distribution testing, CMake script refinement for Windows (MSVC specifics), enhanced error handling & logging implementation, code refactoring & cleanup. | A stable, well-tested version of the software with improved reliability and consistent behavior on all target platforms. | 3-4 weeks |
| **Stage 4: Documentation and Initial Packaging** | Creation of user documentation (covering installation, CLI usage, Web UI usage, troubleshooting), developer documentation (architecture, build process, API overview), basic software packaging (e.g., archives for Linux/Windows). | Comprehensive user and developer documentation; basic packaged versions of the software for easy distribution. | 2-3 weeks |

## **8\. Roadmap for Extended Features**

Following the completion of the initial development stages, which focus on delivering the core P2P file transfer functionality with CLI and web interfaces, several extended features are envisioned. These enhancements aim to improve the software's usability, security, efficiency, and overall user experience.

### **8.1. Text User Interface (TUI) Development**

* **Objective:** To offer a more sophisticated and interactive terminal-based experience, surpassing the capabilities of the basic command-line interface.  
* **Approach:** This will involve the integration of a dedicated C++ Text User Interface library. Promising candidates include:  
  * **FTXUI:** A modern C++ library noted for its functional programming style inspiration, lack of external dependencies, and built-in support for creating interactive UIs with elements like menus, tables, and animations directly in the terminal.14  
  * **TermOx:** A C++20 TUI library that employs a widget-based system, similar in concept to graphical UI toolkits, allowing for structured layout and event handling in the terminal.16  
  * **cpp-terminal:** A library that aims to be a simpler, more lightweight alternative to the traditional ncurses library, focusing on providing cross-platform terminal control functionalities like color manipulation, keyboard input, and resize handling.17

    These libraries provide capabilities far beyond simple iostream-based interactions, enabling the creation of dynamic, visually organized terminal applications.  
* **Potential Features:**  
  * An interactive file and directory browser for visually selecting files or folders to be shared or sent.  
  * A dynamic dashboard displaying ongoing transfers, including real-time progress bars, calculated transfer speeds, estimated time remaining, and the status of connected peers.  
  * User-friendly menus for configuring application settings (e.g., default save locations, network ports), managing the list of shared files, and viewing a history of transfers or connections.  
* The development of a TUI will likely involve adopting programming paradigms different from the basic imperative CLI, such as event-driven or reactive models, depending on the architecture of the chosen TUI library. While this requires learning the library's specific patterns, it can result in a significantly richer and more intuitive user experience for terminal users.

### **8.2. Security Enhancements**

* **Objective:** To safeguard data in transit against unauthorized interception or modification and to provide mechanisms for peers to authenticate each other, ensuring they are communicating with legitimate counterparts.  
* **Approach:**  
  * **Transport Layer Security (TLS):** The cornerstone of data-in-transit security will be the integration of TLS encryption into the TCP communication layer. This will be achieved by utilizing a robust cryptographic library like OpenSSL.20 Asio, the selected networking library, provides built-in support for SSL streams (  
    asio::ssl::stream) that can work seamlessly with OpenSSL to encrypt all P2P communication.5 Similarly, if the web interface is to be served over HTTPS for enhanced security, the chosen HTTP server library (  
    cpp-httplib) also supports SSL integration, typically via OpenSSL.8  
  * **Peer Authentication:** Beyond just encrypting the communication channel, a mechanism for peers to verify each other's identity should be implemented. For LAN environments, this could range from simple methods like pre-shared keys or passphrases. For scenarios requiring higher levels of trust or operation beyond a strictly private LAN, more advanced techniques like certificate-based authentication could be explored.  
* Integrating security features, especially TLS, is a non-trivial undertaking. It involves careful management of cryptographic keys and digital certificates, correct handling of the TLS handshake protocol, and ensuring that all OpenSSL resources are managed meticulously (e.g., by using RAII wrappers to prevent resource leaks or improper state management, which could lead to security vulnerabilities 21). It is highly advisable that architectural considerations for security, such as designing the Core P2P Engine's networking module to easily accommodate an SSL/TLS layer, are part of the initial design phase, even if the full implementation occurs later. Adding security as an afterthought can be significantly more complex and error-prone.

### **8.3. Compression and Decompression**

* **Objective:** To reduce the size of files being transferred, which in turn can decrease transfer times (especially over slower network links) and conserve network bandwidth.  
* **Approach:** This feature will be implemented by integrating a suitable data compression library into the file transfer pipeline. Strong candidates include:  
  * **Zstandard (Zstd):** A modern compression algorithm developed by Facebook, known for its excellent compression ratios and exceptionally high compression and decompression speeds. It is particularly well-suited for real-time compression scenarios where both speed and efficiency are important.22  
  * **zlib:** A widely adopted and highly stable compression library that implements the DEFLATE algorithm (used in formats like GZIP and PNG). It offers a good balance of compression ratio and speed and is a de facto standard in many applications.24  
* **Potential Features:**  
  * An option for the sending peer to enable compression for a file before initiating the transfer. This could be a user-configurable setting or an automatic decision based on file type or size.  
  * Automatic decompression of received files on the recipient peer's side, transparent to the user.  
  * Potentially, a negotiation step in the custom P2P protocol where peers can agree on a common compression algorithm or compression level if multiple options are supported.  
* The implementation of this feature would require modifications to the Core P2P Engine's file handling and transfer protocol modules. Data read from a file on the sender's side would be passed through a compression stream before being chunked and sent over the network. Conversely, data received from the network on the recipient's side would be passed through a decompression stream before being written to the destination file.

### **8.4. LAN Peer Discovery**

* **Objective:** To enable instances of the P2P application running on the same local area network to automatically discover each other. This would eliminate the need for users to manually find and exchange IP addresses, significantly improving the software's ease of use for typical LAN scenarios.  
* **Approach:** This functionality is typically implemented using network broadcast or multicast mechanisms, commonly over UDP.  
  * Peers would periodically send out a small UDP broadcast or multicast packet to a well-known port on the local network. This packet would contain information announcing their presence, such as the application's name, the peer's identifier or display name, and the TCP port on which it is listening for incoming file transfer connections.  
  * Simultaneously, peers would listen for these announcement packets from other instances of the application on the LAN.  
  * Discovered peers could then be listed in the user interface (CLI, TUI, or Web), allowing users to select a peer to connect to without needing to know its IP address beforehand.  
* While not part of the initial core requirements which focus on direct IP address connections, LAN peer discovery is a common and highly desirable feature for P2P applications designed for local network use. Its inclusion in the roadmap is logical for enhancing user experience. The implementation would necessitate adding UDP communication capabilities to the software, which would operate alongside the existing TCP-based file transfer logic.

## **9\. Conclusion and Next Steps**

### **9.1. Summary of Plan**

This document has presented a detailed and structured development plan for a peer-to-peer file transfer software application. The application is designed for cross-platform operation, primarily targeting Arch Linux, general Linux distributions, and Windows, and will be developed using C++20 with CMake as the build system. Core functionality includes direct file transfer over a Local Area Network (LAN) via IP address connections, accessible through both a Command-Line Interface (CLI) and an intuitive web-based user interface. The plan has elaborated on refined project requirements, proposed a modular software architecture centered around a Core P2P Engine, and recommended a technology stack featuring Asio for networking and cpp-httplib for the embedded web server. A custom protocol over raw TCP/IP has been chosen for file transfer, balancing simplicity with control. The development process is broken down into four distinct stages, from an initial proof-of-concept to a documented and packaged application. Finally, a roadmap for future enhancements, including a Text User Interface (TUI), robust security features like TLS encryption, file compression/decompression, and LAN peer discovery, has been outlined to guide long-term development.

### **9.2. Key Recommendations**

To ensure the successful execution of this project, the following key recommendations are emphasized:

* **Prioritize Core Engine Development:** The initial and most critical phase of development should focus on implementing and thoroughly validating the Core P2P Engine. Particular attention must be paid to its TCP networking capabilities using Asio, as this forms the bedrock upon which all other functionalities will be built.  
* **Iterative UI Development:** Commence with the development of a basic CLI. This will facilitate early testing and interaction with the Core Engine. Subsequently, the web interface can be developed, leveraging the well-defined APIs exposed by the Core Engine.  
* **Embrace Modern CMake Practices:** From the project's inception, consistently apply modern CMake principles. This includes using target-based properties (e.g., target\_link\_libraries, target\_include\_directories with appropriate PUBLIC/PRIVATE/INTERFACE scoping), utilizing FetchContent for managing dependencies where appropriate, and adhering to out-of-source build practices. Such practices are crucial for creating a build system that is maintainable, scalable, and truly portable.26  
* **Strategic Library Selection:** Opt for lightweight, well-supported, and actively maintained libraries that align with the project's goals of cross-platform compatibility and ease of integration. For instance, cpp-httplib's predominantly header-only nature can simplify web server integration 8, while Asio provides a robust and portable foundation for network communications.5  
* **Continuous Cross-Platform Focus:** Although primary development might be centered on Arch Linux, it is imperative that regular building and testing of the application occur on Windows and other representative Linux distributions throughout the development lifecycle. This proactive approach, starting from the earliest stages, will help in identifying and addressing platform-specific issues promptly, preventing costly rework later.  
* **Plan for Extensibility and Future Features:** While extended features such as security (TLS) and file compression are part of the longer-term roadmap, their potential integration should inform the architectural design of the Core P2P Engine from the outset. For example, the networking layer should be designed in a way that allows a TLS encryption wrapper to be introduced with minimal disruption. Similarly, the file transfer logic should be structured to accommodate the insertion of compression and decompression streams into the data pipeline. This foresight will facilitate the addition of these advanced features more smoothly in later development stages.

### **9.3. Initiating the Project**

The immediate next step is to formally commence **Stage 1** of the development plan as outlined in Section 7\. The initial tasks will involve:

1. Setting up the defined project directory structure.  
2. Creating the initial CMakeLists.txt files, ensuring they are correctly configured for C++20 and can generate build projects for both Linux and Windows.  
3. Integrating the Asio library into the CMake build system, likely via FetchContent to pin a specific version.  
4. Beginning the implementation of the basic TCP networking layer (client and server connection logic) and the simple application-level protocol within the Core P2P Engine.  
5. Developing the rudimentary CLI to allow for sending and receiving commands, which will serve as the first means of interacting with and testing the core engine.

Concurrently with these initial development tasks, it is recommended that all development team members ensure they are thoroughly familiar with the core APIs, usage patterns, and best practices associated with Asio and the chosen C++ HTTP server library (e.g., cpp-httplib).

### **9.4. Future Collaboration**

This development plan serves as a foundational document to guide the P2P file transfer software project. As development progresses through the outlined stages, ongoing collaboration and detailed discussions will be essential. These discussions will pertain to specific implementation choices within each module, the detailed UI/UX design for the web interface, the precise semantics and state machine of the custom application-level protocol, and the exact mechanisms for implementing security features such as peer authentication and potentially certificate management. Regular reviews of progress against this plan, and refinements to the plan itself, may be undertaken to adapt to any new insights gained or challenges encountered during the development process. This iterative approach to planning and execution will contribute to the project's overall success.

#### **Works cited**

1. FTP vs. TCP: Key Differences and How They Work Together \- GoAnywhere, accessed June 13, 2025, [https://www.goanywhere.com/blog/ftp-vs-tcp-whats-the-difference](https://www.goanywhere.com/blog/ftp-vs-tcp-whats-the-difference)  
2. FTP vs FTPS: Which Protocol Meets Your File Transfer Needs? \- GoAnywhere, accessed June 13, 2025, [https://www.goanywhere.com/blog/ftp-vs-ftps-which-protocol-meets-your-file-transfer-needs](https://www.goanywhere.com/blog/ftp-vs-ftps-which-protocol-meets-your-file-transfer-needs)  
3. Comparison of FTP and TCP file transfer efficiency and improvement programs \- Raysync, accessed June 13, 2025, [https://www.raysync.io/news/comparison-of-ftp-and-tcp-file-transfer-efficiency-and-improvement-programs/](https://www.raysync.io/news/comparison-of-ftp-and-tcp-file-transfer-efficiency-and-improvement-programs/)  
4. Step-by-Step Guide to Eliminating Slow FTP Speeds \- RapidSeedbox, accessed June 13, 2025, [https://www.rapidseedbox.com/blog/slow-ftp-speeds](https://www.rapidseedbox.com/blog/slow-ftp-speeds)  
5. Asio C++ Library, accessed June 13, 2025, [https://think-async.com/](https://think-async.com/)  
6. Socket Programming in C++ | GeeksforGeeks, accessed June 13, 2025, [https://www.geeksforgeeks.org/socket-programming-in-cpp/](https://www.geeksforgeeks.org/socket-programming-in-cpp/)  
7. File Transfer \- TCP \- C++ Forum \- CPlusPlus.com, accessed June 13, 2025, [https://cplusplus.com/forum/general/203524/](https://cplusplus.com/forum/general/203524/)  
8. cpp-httplib \- Crashpad, accessed June 13, 2025, [https://chromium.googlesource.com/crashpad/crashpad/+/439ba730c5ae031195ae927b5f8d077d2d733e77/third\_party/cpp-httplib/cpp-httplib/README.md](https://chromium.googlesource.com/crashpad/crashpad/+/439ba730c5ae031195ae927b5f8d077d2d733e77/third_party/cpp-httplib/cpp-httplib/README.md)  
9. Building RESTful Interfaces in C++ With nlohmann and cpp-httplib \- Kevin Carpenter \- C++ on Sea 2023 \- YouTube, accessed June 13, 2025, [https://www.youtube.com/watch?v=uqPTzUdNLZk](https://www.youtube.com/watch?v=uqPTzUdNLZk)  
10. Setup \- Crow, accessed June 13, 2025, [https://crowcpp.org/0.3/getting\_started/setup/](https://crowcpp.org/0.3/getting_started/setup/)  
11. leo-aa88/crow-tutorial-cpp: Crow microframework tutorial in C++ \- GitHub, accessed June 13, 2025, [https://github.com/leo-aa88/crow-tutorial-cpp](https://github.com/leo-aa88/crow-tutorial-cpp)  
12. etr/libhttpserver: C++ library for creating an embedded Rest HTTP server (and more) \- GitHub, accessed June 13, 2025, [https://github.com/etr/libhttpserver](https://github.com/etr/libhttpserver)  
13. Which libraries to use to create HTTP server on modern C++ (17) : r/cpp \- Reddit, accessed June 13, 2025, [https://www.reddit.com/r/cpp/comments/1jzfkc1/which\_libraries\_to\_use\_to\_create\_http\_server\_on/](https://www.reddit.com/r/cpp/comments/1jzfkc1/which_libraries_to_use_to_create_http_server_on/)  
14. FTXUI: Introduction \- GitHub Pages, accessed June 13, 2025, [https://arthursonzogni.github.io/FTXUI/](https://arthursonzogni.github.io/FTXUI/)  
15. examples/component/input.cpp \- FTXUI \- GitHub Pages, accessed June 13, 2025, [https://arthursonzogni.github.io/FTXUI/examples\_2component\_2input\_8cpp-example.html](https://arthursonzogni.github.io/FTXUI/examples_2component_2input_8cpp-example.html)  
16. a-n-t-h-o-n-y/TermOx: C++20 Terminal User Interface (TUI) Library. \- GitHub, accessed June 13, 2025, [https://github.com/a-n-t-h-o-n-y/TermOx](https://github.com/a-n-t-h-o-n-y/TermOx)  
17. cpp-terminal, accessed June 13, 2025, [https://jupyter-xeus.github.io/cpp-terminal/](https://jupyter-xeus.github.io/cpp-terminal/)  
18. A Brief Introduction to nCurses: Making a Menu \- 1 \- YouTube, accessed June 13, 2025, [https://www.youtube.com/watch?v=fSV4UIpuFV4](https://www.youtube.com/watch?v=fSV4UIpuFV4)  
19. Ncurses Tutorial 0 \- Hello World (initscr, endwin, refresh, getch, printw) \- YouTube, accessed June 13, 2025, [https://www.youtube.com/watch?v=lV-OPQhPvSM](https://www.youtube.com/watch?v=lV-OPQhPvSM)  
20. C++ Programming on Linux \- OpenSSL BIO : Basic I/O Abstraction \- YouTube, accessed June 13, 2025, [https://www.youtube.com/watch?v=kxAdd03wyUo](https://www.youtube.com/watch?v=kxAdd03wyUo)  
21. OpenSSL client and server from scratch, part 1 – Arthur O'Dwyer \- GitHub Pages, accessed June 13, 2025, [https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/](https://quuxplusone.github.io/blog/2020/01/24/openssl-part-1/)  
22. libzstd 1.5.5+1 \- cppget.org, accessed June 13, 2025, [https://www.cppget.org/libzstd/1.5.5+1?f=full](https://www.cppget.org/libzstd/1.5.5+1?f=full)  
23. zstd 1.5.1 Manual, accessed June 13, 2025, [http://facebook.github.io/zstd/zstd\_manual.html](http://facebook.github.io/zstd/zstd_manual.html)  
24. Zip Compression In C++ (zlib) \- HeyCoach | Blogs, accessed June 13, 2025, [https://blog.heycoach.in/zip-compression-in-c-zlib/](https://blog.heycoach.in/zip-compression-in-c-zlib/)  
25. Installing the zlib C library \- YouTube, accessed June 13, 2025, [https://www.youtube.com/watch?v=6as-wCwEYno](https://www.youtube.com/watch?v=6as-wCwEYno)  
26. How to Set Up New Projects on CMake for Success \- Incredibuild, accessed June 13, 2025, [https://www.incredibuild.com/blog/how-to-set-up-new-projects-on-cmake-for-success](https://www.incredibuild.com/blog/how-to-set-up-new-projects-on-cmake-for-success)  
27. Mastering Cross-Platform Development with CMake and C++ \- slaptijack, accessed June 13, 2025, [https://slaptijack.com/programming/cross-platform-development-cmake-and-cpp.html](https://slaptijack.com/programming/cross-platform-development-cmake-and-cpp.html)  
28. CMake Tutorial — CMake 3.20.6 Documentation, accessed June 13, 2025, [https://cmake.org/cmake/help/v3.20/guide/tutorial/index.html](https://cmake.org/cmake/help/v3.20/guide/tutorial/index.html)  
29. Effective Modern CMake \- GitHub Gist, accessed June 13, 2025, [https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1](https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1)  
30. Using Dependencies Guide — CMake 4.0.2 Documentation, accessed June 13, 2025, [https://cmake.org/cmake/help/latest/guide/using-dependencies/index.html](https://cmake.org/cmake/help/latest/guide/using-dependencies/index.html)  
31. cpm-cmake/CPM.cmake: CMake's missing package manager. A small CMake script for setup-free, cross-platform, reproducible dependency management. \- GitHub, accessed June 13, 2025, [https://github.com/cpm-cmake/CPM.cmake](https://github.com/cpm-cmake/CPM.cmake)  
32. Using CMake \- Ontario Tech University, accessed June 13, 2025, [https://csundergrad.science.uoit.ca/courses/cpp-notes/notes/using-cmake.html](https://csundergrad.science.uoit.ca/courses/cpp-notes/notes/using-cmake.html)  
33. csundergrad.science.uoit.ca, accessed June 13, 2025, [https://csundergrad.science.uoit.ca/courses/cpp-notes/notes/using-cmake.html\#:\~:text=CMake%20supports%20a%20number%20of,XCode%20project%20files%20for%20OSX.](https://csundergrad.science.uoit.ca/courses/cpp-notes/notes/using-cmake.html#:~:text=CMake%20supports%20a%20number%20of,XCode%20project%20files%20for%20OSX.)