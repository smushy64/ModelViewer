## TODO

### Future Goals
- Set Window Icon
- DirectX 11
- Vulkan
- Remove glad dependency
- Remove stb dependencies
- Remove tinyobjloader dependency
- Material Editor
- Lights Editor
- Multiple Meshes
- Screenshot
- Other model formats

### Refactor 2
- [x] Win64 layer
  - [x] Window Creation
  - [x] OpenGL Context Creation
- [ ] OpenGL layer
  - [x] Logging
    - [x] Debug Message Callback
    - [x] Extensions
    - [x] Version
    - [x] Graphics card
    - [x] Shading Language version
  - [x] Clear
  - [x] Swap
  - [x] SetClearColor
  - [x] SetViewport
  - [x] Shaders
  - [ ] Vertex Arrays
  - [ ] Vertex Buffers
  - [ ] Index Buffers
  - [ ] Uniform Buffers
  - [x] Texture2D
- [x] Math library
  - [x] vec2
    - [x] magnitude
    - [x] sqrMagnitude
    - [x] normal
    - [x] normalize
    - [x] angle
    - [x] clamp
    - [x] dot
    - [x] scale
    - [x] cmp
    - [x] operator- (negate)
    - [x] operator==
    - [x] operator!=2
    - [x] operator+
    - [x] operator- (subtract)
    - [x] operator* f32
    - [x] operator/ f32
  - [x] ivec2
    - [x] mag
    - [x] sqrmag
    - [x] clamp min max
    - [x] scale
    - [x] cmp
    - [x] operator- (negate)
    - [x] operator==
    - [x] operator!=
    - [x] operator+
    - [x] operator- (subtract)
    - [x] operator* i32
    - [x] operator/ i32
  - [x] vec3
    - [x] magnitude
    - [x] sqrMagnitude
    - [x] normal
    - [x] normalize
    - [x] angle
    - [x] reflect
    - [x] scale
    - [x] dot
    - [x] cross
    - [x] clamp
    - [x] cmp
    - [x] operator- (negate)
    - [x] operator==
    - [x] operator!=
    - [x] operator+
    - [x] operator- (subtract)
    - [x] operator* f32
    - [x] operator/ f32
  - [x] vec4
    - [x] magnitude
    - [x] sqrMagnitude
    - [x] normal
    - [x] normalize
    - [x] cmp
    - [x] operator- (negate)
    - [x] operator==
    - [x] operator!=
    - [x] operator+
    - [x] operator- (subtract)
    - [x] operator* f32
    - [x] operator/ f32
  - [x] mat3
  - [x] mat4
  - [x] quat
- [x] io
  - [x] SetCursorStyle
  - [x] SetCursorVisibility
  - [x] LockCursor
  - [x] Mouse Input
    - [x] Buttons
    - [x] Scroll
    - [x] Pixel Position
    - [x] Screen Position
  - [x] Keyboard Input
  - [x] Load File
  - [x] User Load File
  - [x] Write File
- [ ] Text Rendering
  - [x] Load font
  - [ ] Load font atlas on GPU
  - [ ] Render text
- [x] expand math templates

### Normal Map Support + Fixes and Tweaks - Completed 11/2/2022
- [x] DrawArrays caused segfault
  - I was checking the index buffer pointer to switch between drawarrays and drawelements
  but that pointer was left uninitialized when it was drawarrays so it always caused a segfault
- [x] Consolidate some hpp/cpp
  - [x] parser.hpp/cpp
  - [x] collision.hpp/cpp
  - [x] settings.hpp/cpp
- [x] Loading anything that isn't an OBJ as such caused a segfault
  - Added an extension check before parsing
- [x] Remove api pointer from blinn-phong material
  - now using layout(binding = x) to bind samplers to texture units instead of glActiveTexture(x)
- [x] Normal Maps

### Loading Meshes - Completed 10/31/2022
- [x] Fixed Label Button Bounding Box
- [x] Image loading
- [x] Model loading

### Rendering Meshes + Settings Parsing + Camera Controls - Completed 10/29/2022
- [x] Settings Parser
- [x] Camera Controls
  - [x] Zoom          - Scroll Wheel
  - [x] Rotate Around - Right Click + Mouse
  - [x] Pan           - WASD
- [x] Lights
  - [x] PODs
  - [x] Uniform Buffer
  - [x] Shader Calculations
- [x] Model Shader - Blinn Phong
  - [x] Surface Tint
  - [x] Albedo Texture
  - [x] Specular Texture
  - [x] Shading

### Renderer Improvements + Mouse Pointer + Load File pop up - Completed 10/26/2022
- [x] Update Text Button Bounding Box on screen resolution change
- [x] Uniform Block Buffer
- [x] Mouse Pointer
  - [x] Set style
  - [x] Set visibility
- [x] Separate Renderer API from Renderer
- [x] Texture Loading Abstraction
- [x] Load File from pop up

### UI - Completed 10/22/2022
- [x] Delta time
- [x] Event system
  - [x] Update Viewport when screen resolution changes
  - [x] Capture Input
  - [x] Text Button
- [x] Program crashes when quitting?
  - It was something to do with the default font atlas bitmap, not sure why it crashed when freeing that data but the issue is fixed now
- [x] Memory Leaks?
  - [x] Font shader not being deleted when renderer is deleted?
  - [x] Shader Files not being cleaned up

### Refactor - Completed 10/16/2022

- [x] Win64 layer
  - [x] Window Creation
  - [x] OpenGL Context Creation
- [x] OpenGL layer
  - [x] Clear
  - [x] Swap
  - [x] SetClearColor
  - [x] SetViewport
  - [x] CreateShader
- [x] Text Rendering
  - [x] Load font
  - [x] Load font atlas on GPU
  - [x] Render text

### Project Created > Completed 9/30/2022

- [x] Windows x64 window creation
- [x] Loading file from disk abstraction
- [x] Logging
- [x] Renderer abstraction
- [x] Text rendering
