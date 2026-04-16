# Explorer Custom (expimage)

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows_11-lightgrey.svg)
![Architecture](https://img.shields.io/badge/arch-x64_%7C_ARM64-orange.svg)

A lightweight, safe overlay tool that displays your favorite images (like character sprites or mascots) on top of Windows 11 File Explorer. 

Unlike other tools, this does **NOT** use DLL injection. It uses a pure, safe overlay method that will not break or crash your Explorer after Windows updates.

---

## 🚀 How to Use (Quick Start)

Get your images showing in Explorer in just a few seconds!

### Step 1: Download
1. Go to the **[Releases](https://github.com/kokorodonguri/explorer-custom/releases/latest)** page on this repository.
2. Download the latest executable file (e.g., `expimage-x64-arm64.exe`).
3. Place the `.exe` file into a dedicated folder anywhere on your PC.

### Step 2: Generate the Image Folder
1. Double-click the `.exe` file to run it for the first time.
2. An **`img`** folder will be automatically created right next to your `.exe` file.

### Step 3: Add Your Images
1. Open the newly created `img` folder.
2. Drop your favorite images inside (supported formats: `.png`, `.jpg`, `.jpeg`).
   * *💡 Tip: Transparent PNG files (like character cutouts without backgrounds) look the best!*

### Step 4: Enjoy!
1. Just open or click on any Windows File Explorer window. Your image will smoothly appear in the bottom-right corner of the file list area.
2. If you put multiple images in the `img` folder, the tool will **randomly pick a new image** every time you open or switch to an Explorer window!

### ⌨️ How to Exit
* While an Explorer window is active, simply press **`Ctrl` + `Shift` + `Q`** on your keyboard to completely close the tool.

---

## ✨ Key Features

* **Zero-Lag Tracking:** Uses native Windows Event Hooks to perfectly stick to the Explorer window, even when dragging it around quickly.
* **Smart Positioning:** Automatically detects the main file list area and aligns the image, keeping a perfect margin so it doesn't overlap awkwardly with text.
* **Auto-Kill Previous Instances:** If you accidentally run the `.exe` while it's already running in the background, it will automatically close the old one for you.
* **ARM64 Native Support:** Runs natively and flawlessly on both traditional x64 PCs and modern ARM64 Windows devices.
* **100% Safe:** No registry hacks, no modifying `explorer.exe`, no DLL injections.

## ⚙️ Requirements
* **OS:** Windows 11
* **Architecture:** x64 or ARM64 processors

## 📄 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
