project(RenderGraph)

Module(
    NAME RenderGraph
    TYPE Library
    SRC_PATH  
        /Source/FrontEnd
    DEPS
    DEPS_PUBLIC RuntimeCore
    INCLUDES_PUBLIC  
        /FrontEnd
    LINKS
    LINKS_PUBLIC
)

if(windows)
Module(
    NAME RenderGraphD3D12
    TYPE Library
    SRC_PATH  
        /Source/D3D12
    DEPS
    DEPS_PUBLIC RenderGraph RuntimeCore
    INCLUDES_PUBLIC  
        /D3D12
    LINKS
    LINKS_PUBLIC
)
endif(windows)

if(windows OR macos OR web)
  
if(windows OR macos)
  download_and_extract(
    ${FILE_SERVER_DIR}/binaries/${SAKURA_PLATFORM}/x64/release/dawn.zip
    ${ENGINE_DIR}/Build/PreBuilt/Release/dawn.zip
    ${ENGINE_BIN_DIR}/Release/
  )
  download_and_extract(
    ${FILE_SERVER_DIR}/binaries/${SAKURA_PLATFORM}/x64/debug/dawn.zip
    ${ENGINE_DIR}/Build/PreBuilt/Debug/dawn.zip
    ${ENGINE_BIN_DIR}/Debug/
  )
  set(WGPUBackEnd Dawn)
  set(WGPUNativeLib dawn_native dawn_proc)
else()
  set(WGPUBackEnd Web)
endif()

Module(
    NAME RenderGraphWebGPU
    TYPE Library
    SRC_PATH  
        /Source/WebGPU/Impl
        /Source/WebGPU/${WGPUBackEnd}Impl
    DEPS
    DEPS_PUBLIC RenderGraph RuntimeCore
    INCLUDES_PUBLIC  
        /WebGPU
    LINKS ${WGPUNativeLib}
    LINKS_PUBLIC
)

endif()