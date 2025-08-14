
# Photogate V2

**Version 2 of the Photogate** — implemented with **ESP-IDF v5.4.2** for an ESP32-based photogate project.

---

## Overview

This is the second version of the **Photogate** project, featuring significant improvements in accuracy, reliability, and visual interface, along with available 3D models and schematics.  
Designed for educational and experimental applications using the ESP32 platform.

---

## Requirements

- **ESP-IDF v5.4.2**
- A configured ESP32 development environment (terminal toolchain)
- Required cables and hardware to flash the firmware

---

## Build and Run

1. Clone the repository:
   ```bash
   git clone https://github.com/marciobulla/photogateV2.git
   cd photogateV2
    ```

2. Set up the ESP-IDF environment
   Follow the [Get Started with ESP-IDF](https://docs.espressif.com/projects/esp-idf) guide.

3. Build, flash, and monitor:

   ```bash
   idf.py build
   idf.py flash
   idf.py monitor
    ```

   This will compile the project, upload it to your ESP32, and open the serial monitor.

---

## Repository Structure

* `firmware/` — C source code for ESP32 firmware
* `boards/` — PCB layouts or circuit schematics
* `3DModels/` — Printable or viewable 3D models of the device
* `README.md` — Project documentation (this file)
* Additional helper scripts and documentation

---

## Interactive Bill of Materials (IBOM)

Below is a preview of the Interactive BOM:

🔗 [Open Interactive IBOM](https://photogatev2.marciobulla.com/boards/ibom.html)

<iframe src="./boards/ibom.html" width="100%" height="720px" style="border:none;">
Your browser does not support iframes.
</iframe>


---

## Credits

Developed by **Marcio Bulla**. Inspired by the first Photogate (Optical Timer) version.
Special thanks to the libraries used in this project, such as `encoder_menu.py`, `rotary.py`, and `rotary_irq_esp.py`.
