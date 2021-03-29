{
    "targets": [
        {
            "target_name": "yrp",
            "cflags!": [
                "-fno-exceptions"
            ],
            "cflags_cc!": [
                "-fno-exceptions"
            ],
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
                "NAPI_DISABLE_CPP_EXCEPTIONS"
            ]
        }
    ]
}
