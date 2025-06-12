# p2p-lan-share

# Overview

This project is for us to learn modern C++, developing software in Linux environment and make something useful at the same time. We decide to make a FTP tool that can do direct peer-to-peer file transfer in LAN. The software will need to be implemented in modern C++ where possible, things like rendering a web page can be implemented with other suitable languages.

# Pre-requisites

* Operating System: Arch Linux (latest kernal)
* C++ Version: >= C++20
* Build Tool: Cmake
* Code Editor: NeoVim
* Target Platform: Linux and Windows (MacOS can come later)

# Initial Ideas

## Basic Requirements

These are the basic requirements that need to be implemented first:

* Peer-to-peer file transfer to an ip address on the same LAN
* Support all file formats
* Optimised transfer speed
* Easy to use
* Terminal based software
* User can send and receive files in the terminal software given destination ip addresses
* Software can host a local web server so that other people on the same network can transfer file via a web page (instead of using this software)

## Extended Features

These features can come later:

* The software can scan all ip addresses on the network to see which ip addresses have the same software running and ready to receive files
* Has a TUI/GUI that support file drag and drop, nice and easy to use, sleek design
* File compression and de-compression

## Future Roadmap

Some futuristic ideas:

* File encryption and decryption.
* Use VPN services (e.g. tailscale) to transfer files across internet
* Upload and access (view images, stream videos etc.) files on a server hosted on the same LAN, like a NAS

# Questions

* How does FTP work?
* How can I implement FTP?
* How can I optimise FTP speed and efficency in modern C++?
* What are general good practices that I need to consider for this project?
