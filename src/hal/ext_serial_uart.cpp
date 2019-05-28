/*
  ext_serial_uart.cpp

  Defining extension methods of SerialUART

  Copyright (c) 2019, Shin Hiroe All rights reserved

*/

#include <WioCellLibforArduino.h>

#include <limits.h>
#include "libmrubyc.h"
#include "ext.h"

// parity (0:なし, 1:奇数, 2:偶数)
int get_serial_uart_config(int parity)
{
  if (parity == 0) { return SERIAL_8N1; }
  if (parity == 1) { return SERIAL_8O1; }
  if (parity == 2) { return SERIAL_8E1; }
  return SERIAL_8N1;
}

static void class_serial_uart_initialize(mrb_vm *vm, mrb_value *v, int argc)
{
  if (argc != 1 && argc != 2) {
    DEBUG_PRINT("!!! invalid argc");
    SET_FALSE_RETURN();
    return;
  }

  int speed = GET_INT_ARG(1);
  bool parity  = 0; //パリティなし

  if (argc == 2) {
    parity = GET_INT_ARG(2); // 0:なし 1:奇数 2:偶数
  }

  int config = get_serial_uart_config(parity);
  SerialUART.begin(speed, config);
  SET_TRUE_RETURN();
}

static void class_serial_uart_print(mrb_vm *vm, mrb_value *v, int argc)
{
  if (argc != 1) {
    DEBUG_PRINT("!!! invalid argc");
    SET_FALSE_RETURN();
    return;
  }

  uint8_t *str = GET_STRING_ARG(1);
  SerialUART.print((const char *)str);
}

static void class_serial_uart_read(mrb_vm *vm, mrb_value *v, int argc)
{
  if (argc != 0) {
    DEBUG_PRINT("!!! invalid argc");
    SET_NIL_RETURN();
    return;
  }

  char c = '\0';
  c = SerialUART.read();
  const char str[] = {c, '\0'};
  
  mrbc_value str_obj = mrbc_string_new_cstr(vm, str);
  SET_RETURN(str_obj);
}

static void class_serial_uart_available(mrb_vm *vm, mrb_value *v, int argc)
{
  if (argc != 0) {
    DEBUG_PRINT("!!! invalid argc");
    SET_NIL_RETURN();
    return;
  }

  int size = SerialUART.available();
  SET_INT_RETURN(size);
}

static void class_serial_uart_clear(mrb_vm *vm, mrb_value *v, int argc)
{
  if (argc != 0) {
    DEBUG_PRINT("!!! invalid argc");
    SET_NIL_RETURN();
    return;
  }

  for (int i = 0; i < 10; i++) {
    if (SerialUART.available() > 0) {
      while (SerialUART.available() > 0) { SerialUART.read(); }
    }

    hal_delay(1); // sleep 1ms
  }

  SET_TRUE_RETURN();
}

static void class_serial_uart_gets(mrb_vm *vm, mrb_value *v, int argc)
{
  if (argc != 0 && argc != 1) {
    DEBUG_PRINT("!!! invalid argc");
    SET_NIL_RETURN();
    return;
  }

  char c = '\0';
  char eol = '\r';

  if (argc == 1) {
    uint8_t *str = GET_STRING_ARG(1);
    eol = str[0];
  }

  int i = 0;
  char recv[32];  

  while(c != eol) {
    c = SerialUART.read();
    recv[i] = c;
    i++;
  }

  if (i == 0) {
    SET_NIL_RETURN();
    return;
  }

  recv[i] = '\0';
  mrbc_value str_obj = mrbc_string_new_cstr(vm, recv);
  SET_RETURN(str_obj);
}

void define_serial_uart_class()
{
  mrb_class *class_uart;
  class_uart = mrbc_define_class(0, "SerialUART", mrbc_class_object);

  mrbc_define_method(0, class_uart, "initialize", class_serial_uart_initialize);
  mrbc_define_method(0, class_uart, "print", class_serial_uart_print);
  mrbc_define_method(0, class_uart, "read", class_serial_uart_read);
  mrbc_define_method(0, class_uart, "gets", class_serial_uart_gets);
  mrbc_define_method(0, class_uart, "available", class_serial_uart_available);
  mrbc_define_method(0, class_uart, "clear!", class_serial_uart_clear);
}