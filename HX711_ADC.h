/** @file HX711_ADC.h
 *
 * @brief A library using the HX711 24-bit ADC. Supports multiple HX711 at once.
 *
 * Author: Stefan Milivojcev 23.03.2023
 */

#ifndef HX711_ADC_H
#define HX711_ADC_H

#include <stdint.h>
#include <stdbool.h>

#define HX711_ADC_TIMEOUT_US 10000U

typedef enum{

    HX711_ADC_FAILED = false,
    HX711_ADC_OK = true

}HX711_ADC_OK_t;

typedef enum{

    HX711_ADC_STATUS_BUSY = true,
    HX711_ADC_STATUS_READY = false

}HX711_ADC_Status_t;

typedef enum{

    HX711_ADC_GAIN_128 = 1,
    HX711_ADC_GAIN_64 = 3,
    HX711_ADC_GAIN_32 = 2

} HX711_ADC_Gains_t;

typedef enum{

    HX711_ADC_RATE_10_Hz,
    HX711_ADC_RATE_80_Hz

} HX711_ADC_Rate_t;

typedef enum{

    HX711_ADC_CHANNEL_A,
    HX711_ADC_CHANNEL_B

} HX711_ADC_Channel_t;

typedef struct {

    HX711_ADC_Rate_t rate_select;        //select 10 or 80 Hz
    HX711_ADC_Gains_t gain_select;       //select 128, 64 or 32
    bool (*read_data_pin)(void);            //true - read as high. false - read as low
    void (*write_clock_pin)(bool high_low); //true - set high. false - set low
    void (*delay_us)(uint16_t delay);       // Microsecond delay function

    uint32_t calibration_offset;             //offset will be removed from raw read result
    int32_t zeroing_offset;             //offset will be removed from read result

} HX711_ADC_t;

HX711_ADC_Status_t HX711_ADC_Get_Status(HX711_ADC_t * adc_handle);
int32_t HX711_ADC_Read(HX711_ADC_t * adc_handle);
int32_t HX711_ADC_Read_Average(HX711_ADC_t * adc_handle, uint8_t num_of_averages);
HX711_ADC_OK_t HX711_ADC_Init(HX711_ADC_t * adc_handle);
void HX711_ADC_DeInit(HX711_ADC_t * adc_handle);

#endif //HX711_ADC_H
