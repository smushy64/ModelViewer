## TODO

### Renderer Improvements
- [ ] Uniform Block Buffer

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

### > Completed - 9/30/2022

- [x] Windows x64 window creation
- [x] Loading file from disk abstraction
- [x] Logging
- [x] Renderer abstraction
- [x] Text rendering
