const 
  stm8_src_zeilanz = 13;
  
var
  stm8_templ    : array [0..stm8_src_zeilanz] of string =  
  (
    '/* ---------------------------------------------------------- ',
    '   ---------------------------------------------------------- */',
    '',
    '',
    '/* --------------------------------------------------------------------------- ',
    '                                    M A I N',
    '   --------------------------------------------------------------------------- */',
    'int main(void)',
    '{',
    '  // Mainprogram starts here',
    '  sysclock_init(0);',
    '',
    '  while(1);',
    '}'
  );  
