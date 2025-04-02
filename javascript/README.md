# Iris Codec WebAssembly Module
This repository contains a WebAssembly port of the Iris Codec library, enabling digital pathology slide viewing and processing directly in web browsers.

## Overview
The Iris Codec WebAssembly module provides a JavaScript API to access the core functionality of the Iris Codec library, including:
- Slide file validation
- Metadata extraction
- Tile decompression
- Slide information retrieval

This allows web applications to work with Iris Codec slide files (.iris) without requiring server-side processing. 

## Version
Current version: 2025.1.0

## Building
### Prerequisites
- [Emscripten](https://emscripten.org/docs/getting_started/downloads.html) installed and configured
- [CMake](https://cmake.org/download/) installed
- [Git](https://git-scm.com/downloads) installed

### Build Steps
1. Clone the repository:
```bash
git clone https://github.com/IrisDigitalPathology/Iris-Codec.git
cd Iris-Codec
```
2. Set up the Emscripten environment:
```bash
source /path/to/emsdk/emsdk_env.sh
```
3. Build the WebAssembly module:
```bash
cd javascript
./build.sh
```
The build script will:
- Configure the project with CMake
- Build the WebAssembly module
- Copy the output files to the `examples` directory

### Build Output
The build process generates two main files:
- `iris_codec.wasm`: The WebAssembly binary module.
- `iris_codec.js`: The JavaScript bindings for the WebAssembly module.
These files will be copied to the `examples/js/` directory.

## Usage
### Basic Integration
Include the JavaScript file in you r HTML:
```html
<script src="iris_codec.js"></script>
```
Initialize the module in your JavaScript code:
```javascript
IrisCodec().then(module => {
    // Get codec version
    const version = module.getCodecVersion();
    console.log(`Codec Version: ${version.major}.${version.minor}.${version.build}`);
    
    // Validate a slide file
    const result = module.validateSlide("path/to/slide.iris");
    if (result.success) {
        console.log("Slide is valid");
    } else {
        console.log(`Validation failed: ${result.message}`);
    }
}).catch(err => {
    console.error("Failed to initialize module:", err);
});
```
### Working with Files
To work with slide files, you need to use the Emscripten file system API:
```javascript
IrisCodec().then(module => {
    // Write a file to the virtual filesystem
    const fileData = new Uint8Array(arrayBuffer); // From File API or fetch
    module.FS.writeFile("slide.iris", fileData);
    
    // Validate the slide
    const result = module.validateSlide("slide.iris");
    
    // Get slide information
    if (result.success) {
        const info = module.getSlideInfo("slide.iris");
        console.log(`Dimensions: ${info.extent.width}x${info.extent.height}`);
        console.log(`Layers: ${info.extent.layers.length}`);
    }
}).catch(err => {
    console.error("Failed to initialize module:", err);
});
```

## API Reference
### Module Initialization
```javascript
IrisCodec().then(module => { ... });
```

### Functions
`getCodecVersion()`
- Returns the version of the codec as an object with `major`, `minor`, and `build` properties.

`validateSlide(filePath)`
- Validates an Iris Codec slide file.
- Parameters:
  - `filePath`: The path to the slide file.
- Returns:
  - An object with `success` and `message` properties.
  - `success` is `true` if the slide is valid, `false` otherwise.
  - `message` contains an error message if validation fails.

`getSlideInfo(filePath)`
- Retrieves information about an Iris Codec slide file.
- Parameters:
  - `filePath`: The path to the slide file.
- Returns:
  - An object with `extent` and `layers` properties.
  - `extent` is an object with `width`, `height`, and `layers` properties.
  - `layers` is an array of objects with `name` and `index` properties.

### File System API
The module exposes the Emscripten file system API through `module.FS`:
- `FS.writeFile(path, data)`: Write data to a file
- `FS.readFile(path, opts)`: Read data from a file
- `FS.unlink(path)`: Delete a file
- `FS.mkdir(path)`: Create a directory

## Examples
The `examples/js/` directory contains a simple HTML/JavaScript example that demonstrates how to use the Iris Codec WebAssembly module.

### Basic Example
```html
<!DOCTYPE html>
<html>
<head>
    <title>Iris Codec WebAssembly Test</title>
</head>
<body>
    <h1>Iris Codec WebAssembly Test</h1>
    <div id="output"></div>

    <script>
        function log(message) {
            document.getElementById('output').innerHTML += `<p>${message}</p>`;
        }

        IrisCodec().then(module => {
            log("Module loaded successfully!");
            
            const version = module.getCodecVersion();
            log(`Codec Version: ${version.major}.${version.minor}.${version.build}`);
        }).catch(err => {
            log(`Failed to load module: ${err}`);
        });
    </script>
    <script src="iris_codec.js"></script>
</body>
</html>
```

### File Upload Example
See `examples/js/tile-viewer.html` and `examples/js/slide-viewer.html` for complete examples of uploading and processing slide files.
- serve using `python -m http.server` or similar
- navigate to `http://localhost:8080/tile-viewer.html` or `http://localhost:8080/slide-viewer.html` in a web browser
- select a slide file and click "Validate Slide" and "Get Slide Info" to test output