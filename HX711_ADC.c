/** @file HX711_ADC.c
 *
 * @brief A library using the HX711 24-bit ADC. Supports multiple HX711 at once.
 *
 * Author: Stefan Milivojcev 23.03.2023
 */

#include "HX711_ADC.h"

HX711_ADC_Status_t HX711_ADC_Get_Status(HX711_ADC_t * adc_handle)
{
    return adc_handle->read_data_pin();
}

/*Pulses the clock pin a number of times depending on the gain_select*/
static void HX711_ADC_Set_Gain(HX711_ADC_t * adc_handle)
{

    for (uint8_t i = 0; i < adc_handle->gain_select; ++i) {
        adc_handle->write_clock_pin(true);
        adc_handle->delay_us(1u);
        adc_handle->write_clock_pin(false);
        adc_handle->delay_us(1u);
    }

}
static uint32_t HX711_ADC_Read_Raw(HX711_ADC_t * adc_handle)
{
    uint32_t raw_adc_reading = 0;

    //read 24 bits
    for (uint8_t i = 0; i < 24; ++i) {
        adc_handle->write_clock_pin(true);
        adc_handle->delay_us(1u);
        raw_adc_reading |= adc_handle->read_data_pin() ? 1 : 0;
        raw_adc_reading <<= 1;
        adc_handle->delay_us(1u);
        adc_handle->write_clock_pin(false);
        adc_handle->delay_us(1u);
    }

    HX711_ADC_Set_Gain(adc_handle);//pulse the last x bits for gain selection (for next read)

    return raw_adc_reading - adc_handle->calibration_offset;
}

int32_t HX711_ADC_Read(HX711_ADC_t * adc_handle)
{
    uint8_t padding = 0xFF;

    uint32_t raw_adc_reading = HX711_ADC_Read_Raw(adc_handle);

    //Perform two's complement conversion, required by the datasheet
    raw_adc_reading ^= 0xFFFFFF;//24 bit inversion

    // Padding for a 32-bit signed integer
    if ((  raw_adc_reading & 0x80000 )
       || (raw_adc_reading == 0x7FFFFF) )
    {
        padding = 0xFF;
    } else {
        padding = 0x00;
    }

    raw_adc_reading = (raw_adc_reading | (padding << 24));
    raw_adc_reading++;
    int32_t result = ((int32_t)raw_adc_reading) - adc_handle->zeroing_offset;
    return result;
}

int32_t HX711_ADC_Read_Average(HX711_ADC_t * adc_handle, uint8_t num_of_averages)
{
    int64_t sum = 0;
    for (uint8_t i = 0; i < num_of_averages; ++i) {
        sum += HX711_ADC_Read(adc_handle);
    }
    return (sum / num_of_averages);
}

HX711_ADC_OK_t HX711_ADC_Init(HX711_ADC_t * adc_handle)
{
    uint32_t timeout_cnt = 0;

    adc_handle->write_clock_pin(false);
    while (HX711_ADC_Get_Status(adc_handle) != HX711_ADC_STATUS_READY)
    {
        if (timeout_cnt++ >= HX711_ADC_TIMEOUT_US) return HX711_ADC_FAILED;
        else adc_handle->delay_us(1);
    }

    //24 + gain_select since it sets the gain for the next read
    for (uint8_t i = 0; i < (24 + adc_handle->gain_select); ++i) {
        adc_handle->write_clock_pin(true);
        adc_handle->delay_us(1u);
        adc_handle->write_clock_pin(false);
        adc_handle->delay_us(1u);
    }


    return HX711_ADC_OK;
}

void HX711_ADC_DeInit(HX711_ADC_t * adc_handle)
{
    adc_handle->write_clock_pin(false);
    adc_handle->write_clock_pin(true);
    adc_handle->delay_us(60u);
}

