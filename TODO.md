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
- Screenshot feature
- Other model formats
- Font Atlases at different point sizes
- Reorder struct members to prevent unnecessary padding bytes
- settings.ini
  - camera sensitivity

### Minor Bugs
- Clicking and holding while moving mouse out of window, letting go and then coming back to window registers a click anyway
- Camera stutter when looking completely up/down

### Refactor 2
- [x] Quad is not drawing properly? 
  - Not sure why but index order was what was causing it. I copied the same meshes from before the refactor so that shouldn't have been the problem but anyway, it's solved now.
- [x] Review every call to "strLen"
  - It reports the string length up to but not including the null-terminator
  - Also rewrite strcopy so that it respects the bounds of the str ( non-inclusive maxDstLen )
  - Ended up rewriting all string-related functions in util.hpp, turns out char buffers are hard to deal with.
    Who woulda thunk?
- [x] Delete copy constructor for Platform::VertexBufferLayout, make it move only
  - Got rid of construct/destructor instead, too much of a hassle so i made vertex buffer layout a pod
- [x] Win64 layer
  - [x] Window Creation
  - [x] OpenGL Context Creation
- [x] OpenGL layer
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
  - [x] Vertex Arrays
  - [x] Vertex Buffers
  - [x] Index Buffers
  - [x] Uniform Buffers
  - [x] Texture2D
  - [x] Draw Elements
  - [x] Draw Array
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
- [x] Text Rendering
  - [x] Load font
  - [x] Load font atlas on GPU
  - [x] Render text
- [x] expand math templates
- [x] Recreate UI from before refactor
  - [x] break ui out to separate header/source
  - [x] OOP so far has made a bit of sense here, might not work out too great with alloc/free char buffer but we'll see
    - works ok, twice the work of just using structs tho lol
- [x] Loading images
- [x] Loading objs
- [x] Model viewing!
- [x] integrate new smath library

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

### Refactor 1 - Completed 10/16/2022

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
