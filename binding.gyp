{
  "targets": [
    {
      "target_name": "ll_keyboard_hooks",
      "sources": [ "ll_keyboard_hooks.cc" ],
      'include_dirs': [
        "<!(node -p \"require('node-addon-api').include_dir\")",
      ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'xcode_settings': {
      'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
      'CLANG_CXX_LIBRARY': 'libc++',
      'MACOSX_DEPLOYMENT_TARGET': '10.7',
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      },
      'conditions': [
        ['OS=="win"', {
          'defines': [
            'OS_WIN',
            # _HAS_EXCEPTIONS must match ExceptionHandling in msvs_settings.
            '_HAS_EXCEPTIONS=1',
          ],
        }],
        ['OS=="mac" or OS=="linux"', {
          'defines': [
            'OS_POSIX',
          ],
        }],
        ['OS=="linux"', {
          'cflags':[
            # Don't warn about the "struct foo f = {0};" initialization pattern.
            '-Wno-missing-field-initializers',
          ],
        }],
      ],
    }
  ]
}
