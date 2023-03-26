# HX711-Universal-Library
A universal C library for the HX711 ADC, independant of the microcontroller vendor. Any microcontroller’s GPIO pins should work. The user only needs to write 3 small functions. Two functions, for writing and reading to and from a pin, and a microsecond delay function. These functions are then hooked up to the library using function pointers. Many HX711 ADCs can be used at once, since they would use different functions for pin reading and writing. 

The function examples would be:
```C
void delay_us(uint16_t delay)
{
    TIM6->CNT = 0; // this timer would be constantly running
    while (TIM6->CNT < delay);
}

void write_hx711_clock_1(bool level)
{
    switch (level) {
    case true:
        GPIOA->ODR |= 1 << 9;
        break;
    default:
        GPIOA->ODR &= ~(1 << 9);
        break;
    }

}

bool read_hx711_data_1(void)
{

    return (GPIOA->IDR & (1<<8)) != 0x00u;

}
```

Then just initialize the main HX711 struct i.e.:
```C
  HX711_ADC_t test_hx711 = { 0 };
  test_hx711.delay_us = &delay_us;
  test_hx711.read_data_pin = &read_hx711_data_1;
  test_hx711.write_clock_pin = &write_hx711_clock_1;
  test_hx711.gain_select = HX711_ADC_GAIN_128;
  
  HX711_ADC_Init(&test_hx711);
```

Finally, read the result with a function like:
```C
int32_t read_res = HX711_ADC_Read(&test_hx711);
```
