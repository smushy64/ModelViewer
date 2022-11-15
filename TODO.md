## TODO

### Refactor 2
- [ ] Win64 layer
  - [ ] Window Creation
  - [ ] OpenGL Context Creation
- [ ] OpenGL layer
  - [ ] Clear
  - [ ] Swap
  - [ ] SetClearColor
  - [ ] SetViewport
  - [ ] CreateShader
- [ ] Text Rendering
  - [ ] Load font
  - [ ] Load font atlas on GPU
  - [ ] Render text

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
