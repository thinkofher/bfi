project('bfi', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

src = [
  'main.c', 'bfi.c',
]

exe = executable('bfi', src,
  install : true)

test('basic', exe)
