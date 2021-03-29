{
    "targets": [
        {
            "target_name": "yrp",
            "cflags!": ["-fno-exceptions"],
            "cflags_cc!": ["-fno-exceptions"],
            "sources": [
                "lib/binding.cc",
                "lib/buffer.cc",
                "lib/replay.cc",
                "lib/lzma/Alloc.cc",
                "lib/lzma/LzFind.cc",
                "lib/lzma/LzmaDec.cc",
                "lib/lzma/LzmaEnc.cc",
                "lib/lzma/LzmaLib.cc"
            ],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")"
            ],
            "libraries": [],
            "dependencies": [
                "<!(node -p \"require('node-addon-api').gyp\")"
            ],
            "defines": [
                "NAPI_CPP_EXCEPTIONS"
            ],
            "conditions": [
                ["OS==\"mac\"", {
                    "cflags+": ["-fvisibility=hidden"],
                    "xcode_settings": {
                        "GCC_SYMBOLS_PRIVATE_EXTERN": "YES",
                        "GCC_ENABLE_CPP_EXCEPTIONS": "YES"
                    }
                }],
                ["OS==\"linux\"", {
                    "defines": ["LINK_PLATFORM_LINUX=1"],
                    "cflags": ["-fexceptions"],
                    "cflags_cc": ["-fexceptions"]
                }],
                ["OS==\"win\"", {
                    "msvs_settings": {
                        "VCCLCompilerTool": {
                            "ExceptionHandling": "2"
                        }
                    }
                }]
            ]
        }
    ]
}
