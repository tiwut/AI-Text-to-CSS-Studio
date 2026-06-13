# AI Text-to-CSS Studio

A high-performance C++ desktop application built with Qt6 that converts textual prompts into static vector graphics using text-only LLMs (such as local Ollama, Google Gemini, or OpenAI models) by having them write pure, self-contained HTML/CSS/SVG layouts.

Because the output graphic is vector-based, the application can render and export the illustration off-screen at arbitrary dimensions (e.g., HD, 2K, 4K, 8K, or custom resolutions) with absolute clarity and zero pixelation.

---

## Key Features

- **Multi-Provider REST Clients**: Integration with Google Gemini, OpenAI, and local Ollama API endpoints. 
- **Auto-discovery**: Queries local Ollama registries dynamically on startup to list local models in the dropdown.
- **High-Resolution Vector Export**: Sets off-screen render engine viewports to custom sizes (e.g. 3840x2160) to render and save crisp high-fidelity PNG/JPG illustrations.
- **Real-Time Workspace Sync**: Real-time debounced updates render code edits from the editor tab into the live canvas view automatically.
- **Syntactical Code Highlights**: A custom syntax highlighter that formats HTML tags, attributes, strings, properties, and values.
- **Premium QSS Dark Theme**: Fully styled split-pane layouts, combo boxes, settings panels, custom scrollbars, and glowing UI components.
- **Comment-Free Pure Code**: Highly polished C++17 source files containing clean, optimized execution blocks.

---

## Project Structure

```text
├── CMakeLists.txt         # CMake build configuration
├── main.cpp               # Application setup and DPI scale rounding configurations
├── config.hpp/cpp         # QSettings configuration wrapper
├── ai_client.hpp/cpp      # Async HTTP REST client for OpenAI, Gemini, and Ollama APIs
├── preview_view.hpp/cpp   # Canvas viewport and offscreen export capture engine
├── code_editor.hpp/cpp    # QTextEdit editor with line numbers and QSyntaxHighlighter
├── settingsdialog.hpp/cpp # API Key credentials setup panel
├── mainwindow.hpp/cpp     # Main layout, custom style sheet, and event connections
└── test_export.cpp        # Headless testing executable for offscreen exports
```

---

## Build Requirements

- **C++ Compiler**: A modern C++17 compiler (GCC 9+, Clang 10+, or MSVC 2019+).
- **Qt6 SDK**: Includes `Core`, `Widgets`, `Network`, and `WebEngineWidgets`.
- **CMake**: Version 3.16 or higher.

### Installing Dependencies (Ubuntu/Debian example)

```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-webengine-dev pkg-config
```

---

## How to Build

1. **Configure the build folder**:
   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   ```
2. **Compile the executable targets**:
   ```bash
   cmake --build build -j$(nproc)
   ```

This compiles two binaries in `build/`:
- `ai_txt_to_picture`: The main desktop GUI application.
- `test_export`: A headless automated testing tool to verify offscreen rendering pipelines.

---

## Running the Application

### Desktop GUI App

Start the main studio interface:
```bash
./build/ai_txt_to_picture
```

### Headless Verification Test

To verify offscreen graphics rendering without opening the GUI window:
```bash
./build/test_export
```
This renders a gradient vector badge offscreen and saves a sample PNG at `/test_export_output_cpp.png`.

---

## License

This project is open-source and available under the [MIT License](LICENSE).
