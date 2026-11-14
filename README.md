# Opensource smart gardening system

* Multi-sensor temperature data logging
* Advanced multi-purpose scheduler (can used for irrigation pump, solenoid valves and etc...)
* Manual and Auto Time/Date Config (+External RTC Support)
* Cloud-based (Firebase) data storing
* Manual GPIO control (e.g. Can be used for turning on a pump manualy)
* Remote direct connection between nodes (Coming soon)
* Based on PlatformIO

# How to Install

Follow these steps to set up, build, and run Intelligarden on your ESP32.

## 1. Requirements

Install the following tools:

- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO IDE](https://platformio.org/install/ide?install=vscode)
- [Git](https://git-scm.com/downloads)
- [Node.js (LTS)](https://nodejs.org/) (includes npm)

## 2. Clone the Repository

Clone the repository recursively to automatically download all Git submodules:

```bash
git clone --recurse-submodules <repository-url>
cd The-Intelligarden-Project
```

**Important:** Do not omit `--recurse-submodules`. The project depends on Git submodules that must be initialized and downloaded.

If you have already cloned the repository without this option, run:

```bash
git submodule update --init --recursive
```

## 3. Install Web Application Dependencies

The web application is built using React and Node.js.

Navigate to the web application directory and install its dependencies:

```bash
cd intelligarden-web-app
npm install
```

Return to the project root:

```bash
cd ..
```

## 4. Build and Upload

Open the project in Visual Studio Code and allow PlatformIO to initialize the environment and install the required dependencies.

### Upload Firmware

Connect your ESP32 board through USB and run:

```bash
pio run -t upload
```

### Upload Filesystem

Upload the SPIFFS filesystem:

```bash
pio run -t uploadfs
```

**Automated Web Build:** You don't need to build the React application manually. The PlatformIO build script (`WebBuildScript.py`) automatically:

1. Builds the React application using `npm run build`.
2. Removes the previous `data/dist` directory.
3. Copies the newly generated React application into `data/dist`.
4. Allows PlatformIO to generate the SPIFFS filesystem image.

This process is triggered by the SPIFFS image build through PlatformIO's pre-build hook.

**Important:** Uploading the filesystem overwrites its existing contents. Back up device-specific configuration files before uploading a new filesystem image.

## 5. First Boot and Initial Configuration

After uploading the firmware and filesystem, restart your ESP32.

On its first boot, the device starts a Wi-Fi hotspot:

| Setting | Value |
|---|---|
| SSID | `intelligarden-development` |
| IP Address | `192.168.4.1` |

### Connect to Intelligarden

1. Connect your computer or phone to the `intelligarden-development` Wi-Fi hotspot.
2. Open a web browser.
3. Navigate to:

   `http://192.168.4.1`

4. Log in using the default credentials:

| Setting | Value |
|---|---|
| Username | `admin` |
| Password | `12345654321` |

After logging in, you can:

- Configure Intelligarden to connect to your Wi-Fi access point.
- Configure the device settings.
- Complete the remaining system configurations.

Once configured, Intelligarden can connect to your selected Wi-Fi network.

## 6. EnumCrafter: Updating Enums

The project uses [EnumCrafter](<enumcrafter-repository-url>) to generate enum metadata and header files.

Whenever you modify an existing enum class or add a new enum class, you must regenerate the generated files.

In Visual Studio Code:

1. Open the Command Palette or the Terminal menu.
2. Select **Run Task**.
3. Execute:

   `EnumCrafter Update Metadata and Header File`

This updates the enum metadata and generated header file to reflect your changes.

Remember to run this task after modifying or adding enum classes.

## 7. Serial Monitor

To access ESP32 serial output:

```bash
pio device monitor
```

## 8. Project Structure

```text
The-Intelligarden-Project/
│
├── src/                    # ESP32 firmware source
├── include/                # Header files
├── lib/                    # Project libraries
├── tools/                  # Development tools and submodules
│
├── data/                   # SPIFFS filesystem contents
│   ├── dist/                # Generated React application
│   └── ...                  # Configuration files
│
├── intelligarden-web-app/  # React web application
├── partitions/              # Custom partition tables
├── WebBuildScript.py        # Automated web build script
├── platformio.ini           # PlatformIO configuration
└── README.md
```

## Notes

- PlatformIO automatically installs the required framework and declared library dependencies.
- The project uses a custom partition table. Do not replace it with the default ESP32 partition layout.
- SPIFFS contains both web application files and device configuration files.
- The React application is automatically built during filesystem image generation.
- Run `npm install` whenever web application dependencies need to be installed or updated.
- Remember to update generated enum files using the EnumCrafter VS Code task after modifying enum classes.

# How to contribute

1. Fork the Repository: This creates a personal copy of this repository.
2. Clone the Fork: you should clone the fork to your local machine. 
`git clone https://github.com/your-username/forked-repo.git`
3. To keep your fork up-to-date with this repository, you can add the main repo as a remote: `git remote add upstream https://github.com/mahyar1284/The-Intelligarden-Project.git`. This command adds the main repository as a remote named upstream.
4. Fetch the latest changes from the main repo: `git fetch upstream`
5. Merge the changes into your local branch: `git checkout main
git merge upstream/main` This command switches to the main branch and merges the changes from the upstream/main branch into your local main branch.
6. Push the updated branch to your fork: `git push origin main`
7. To always keep your repo up-to-date, do the procedure from no.4 to no.6 everytime you want to create a new branch in your repo

### Create a branch to add new changes

1. Create a Branch: You should create a new branch for your changes.
`git checkout -b feature-branch-name`
2. Make Changes: You can now make changes to the code.
3. Commit Changes: You should commit Your changes.
```
git add .
git commit -m "Description of changes"
```
4. Push Changes: You should push Your branch to your fork. `git push origin feature-branch`
5. Create a Pull Request: You should go to the original repository and create a pull request from your branch (that is on your fork repo).
6. Wait for me to review your pull request and actually merge it to the development branch.


# Library Dependencies & versions
1. fbiego/ESP32Time@^2.0.6
2. paulstoffregen/OneWire@^2.3.8
3. milesburton/DallasTemperature@^3.11.0
4. arduino-libraries/NTPClient@^3.2.1
5. esphome/ESPAsyncWebServer-esphome@^3.2.2
6. bblanchon/ArduinoJson@^7.1.0
7. olikraus/U8g2@^2.35.19
8. mobizt/Firebase ESP32 Client@4.4.14

# Software Design Map (Main UML Class Diagram)
![UML-Diagram](https://github.com/user-attachments/assets/92c3151d-2e2d-40f5-99a5-e614d2298b24)
