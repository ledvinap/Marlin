// following code will produce warning 'initializer element is not a constant expression'
// only way to suppress this warning (that I am aware of) is to disable all warning by treating this file as system header.
// this file contains only table initialization - all other thermistor_template will run with warnings enabled
// comment this out when modifying this file or thermistor_template.h and check all warnings
#pragma GCC system_header

  __entry(350),
  __entry(300),
  __entry(250),
  __entry(200),
  __entry(150),
  __entry(100),
  __entry(50),
  __entry(0),
