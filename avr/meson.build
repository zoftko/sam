lcd = library('lcd', 'lcd.c', include_directories: inc)
interactive_tx = executable(
  'interactive_tx',
  'interactive_tx.c',
  include_directories: inc,
  name_suffix: 'elf',
  link_with: [tx, lcd, bits]
)

objcopy = find_program('avr-objcopy')
interactive_tx_hex = custom_target(
  'interactive_tx_hex',
  input: interactive_tx,
  output: 'interactive_tx.hex',
  command: [objcopy, '-j', '.text', '-j', '.data', '-O', 'ihex', '@INPUT@',  '@OUTPUT@'],
  build_by_default: true
)
