## TODO

### Loading Meshes
- [ ] Model loading abstraction

### Rendering Meshes + Settings Parsing
- [x] Settings Parser
- [ ] Camera Controls
  - [x] Zoom          - Scroll Wheel
  - [ ] Rotate Around - Right Click + Mouse
  - [ ] Pan           - WASD
- [ ] Lights
  - [x] PODs
  - [x] Uniform Buffer
  - [ ] Shader Calculations
- [ ] Model Shader - Blinn Phong
  - [ ] Albedo Texture
  - [ ] Specular Texture
  - [ ] Normal Map

### Renderer Improvements + Mouse Pointer + Load File pop up
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

### Total Rewrite - Completed 10/16/2022

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
