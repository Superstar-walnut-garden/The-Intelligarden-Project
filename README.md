# SANDFLUSH PRO
### Advanced Multi-Filter Backwash Control System

An intelligent, configurable, and user-friendly automatic backwash controller for agricultural irrigation filtration systems.

SandFlush Pro is based on the [IntelliGarden Project](https://github.com/Superstar-walnut-garden/The-Intelligarden-Project), and aims to combine the capabilities of professional commercial filtration controllers with more advanced automation, flexible configuration, intelligent sequencing, and a modern web-based interface.

## Key Features

- **Automatic Backwashing:** Differential-pressure-triggered, time-based, and manual backwash operation.
- **Multi-Filter Management:** Configurable filter sequencing, valve control, and backwash timing.
- **Alternating Backwash Priority:** Automatically alternates the first filter in each backwash cycle to balance sequencing priority between two filters.
- **Advanced Monitoring:** Pressure monitoring, system status, alarms, and fault detection.
- **Flexible Automation:** Configurable operating parameters, delays, safety interlocks, and recovery logic.
- **Modern Web Interface:** Intuitive system monitoring, configuration, and manual control.

## Technology Stack

- **Firmware:** C++
- **Web Application:** TypeScript, React

## EnumCrafter Code Generation

Initialize the EnumCrafter submodule when cloning:

```bash
git clone --recurse-submodules https://github.com/mahyar1284/sandflush-pro.git
```

For an existing checkout, run `git submodule update --init --recursive`. Install
the Python dependency and run the tool from the project root:

```bash
python -m pip install -r tools/EnumCrafter/requirements.txt
python tools/EnumCrafter/main.py
```

The default config is the project-root `config.json`. Its `output` value is a
directory where both `enum_metadata.json` and `EnumCrafter.hpp` are generated.
Set `namespace` to choose the namespace for the generated functions (default:
`EnumCrafter`). Pass a different config path to override it, or use
`--metadata-only` to skip header generation:

```bash
python tools/EnumCrafter/main.py path/to/config.json
python tools/EnumCrafter/main.py --metadata-only
```

## Contributing

Contributions, ideas, bug reports, and feature suggestions are welcome! Feel free to open an issue to discuss improvements or submit a pull request with your changes. All contributions should prioritize reliability, maintainability, and practical real-world irrigation applications.

## Development Vision

SandFlush Pro aims to deliver the capabilities of professional commercial backwash controllers while going further in flexibility, automation, monitoring, and user experience.

**Built for reliable filtration. Designed for intelligent automation.**

<badge color="warning">Under Development</badge>

*An independent project, not affiliated with or endorsed by Netafim.*
