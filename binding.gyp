{
  "targets": [
    {
      "target_name": "pitft",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "xcode_settings": { "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
        "CLANG_CXX_LIBRARY": "libc++",
        "MACOSX_DEPLOYMENT_TARGET": "10.7",
      },
      "msvs_settings": {
        "VCCLCompilerTool": { "ExceptionHandling": 1 },
      },
      "conditions": [
        ['OS=="linux"', {
          "include_dirs": [
            "<!@(node -p \"require('node-addon-api').include\")",
            "<!@(pkg-config cairo --cflags-only-I | sed s/-I//g)"
          ],
          "sources": [ "src/pitft.cc", "src/framebuffer.cc", "src/framebufferWrapper.cc" ],
          "libraries": [
            "<!@(pkg-config cairo --libs)"
          ]
        }]
        ]
    }
  ]
}
