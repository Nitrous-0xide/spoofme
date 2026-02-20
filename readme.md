# SPOOFME // Identity Shifter 2026

![C++](https://img.shields.io/badge/Language-C%2B%2B-blue.svg) 
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)
![License](https://img.shields.io/badge/License-MIT-purple.svg)

**SPOOFME** is a high-performance Windows utility designed for hardware identity obfuscation. It automates the process of scrambling unique machine identifiers and network signatures to bypass tracking or hardware-based identification systems.

---

## 🛠 Features

* **Dynamic MAC Spoofing:** Scans active network interfaces via `SetupAPI`, filters out virtual miniports, and assigns randomized, valid MAC addresses.
* **Registry GUID Scrambling:** Modifies the `MachineGuid` within the Windows Cryptography subkeys to break software-to-hardware linking.
* **Hardware Profile Randomization:** Updates the `HwProfileGuid` to shift the system's hardware configuration ID.
* **Hardware Re-initialization:** Forces a property change via `DIF_PROPERTYCHANGE` so that network changes take effect without a system reboot.
* **Privilege Awareness:** Built-in SID checking to ensure the process is running with necessary Administrative tokens.



## 🏗 Technical Overview

The application interacts directly with the Windows Registry and Driver Management APIs to perform "live" identity shifts.

### Targeted Identifiers:
| Component | Registry / API Path |
| :--- | :--- |
| **MAC Address** | `Control\Class\{4d36e972-e325-11ce-bfc1-08002be10318}` |
| **Machine GUID** | `SOFTWARE\Microsoft\Cryptography` |
| **HWID Profile** | `SYSTEM\CurrentControlSet\Control\IDConfigDB\Hardware Profiles` |



## 🚀 Getting Started

### Prerequisites
* **Windows OS** (x64 recommended)
* **Visual Studio 2022+** with C++ Desktop Development.
* **Admin Rights:** The application must be launched as Administrator to write to `HKEY_LOCAL_MACHINE`.

### Compilation
1. Open the project in Visual Studio.
2. Build as **Release | x64**.

## 💻 Usage

1.  Run the compiled `.exe` as **Administrator**.
2.  The "Boot Sequence" will verify your OS build and permissions.
3.  The program will automatically iterate through network adapters and registry keys.
4.  Once finished, your identity traces are obfuscated. 

## ⚠️ Disclaimer
Modifying hardware identifiers can lead to software licensing issues or network instability. Use at your own risk.

---
> *"Nitroud is a skid" - Nitrous 2026*
