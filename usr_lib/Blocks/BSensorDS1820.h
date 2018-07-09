/* DS1820 temperature sensor block
 * The conversion process of DS1820 sensor takes at least 750ms by the specefifcations.
 * This block divides the DS1820 sensor useage into two step:
 * 1. Conversion issue by startConversion() method
 * 2. Reading conversion result from sensor by readTemperatrue() method
 * The result is stored in 'temperature' property of this class. The user
 * can get this value by get() method.
 * 
 * 1. Undependent usage of this block:
 * 
 * startConversion();
 * delay(800);
 * if (dataReady() == DS1820_STAT_DATA_READY) {
 *     get(0, &temp);
 * }
 * 
 * Also possible:
 * 
 * startConversion();
 * while(dataReady() != DS1820_STAT_DATA_READY) {sleep(200);}
 * get(0, &temp);
 * 
 * 2. Block connection usage:
 * The conversion is started through calling trigger() method.
 * If trigger_caller_block pointer is passed to the method,
 * the *trigger_caller_block will be called after conversion completed.
 * In order to automatically read sensor and trigger the trigger_caller_block after conversion finished,
 * this block has to be 'connected' to BlockTimer using setTimer() method.
 * 
 * BlockTiming TIMING;  //in global scope
 *
 * //In your SETUP procedure:
 * BlockBase* DS1820 = new BSensorDS1820(12, TIMING);  //DS1820 will create his own Timer using BlockTiming functionality
 * 
 * //In the infinite LOOP must be:
 * TIMING.update();
 */

#ifndef BSensorDS1820_h
#define BSensorDS1820_h

#include <Arduino.h>
#include "BlockBase.h"
#include "OneWire.h"
#include "BlockSystem.h"
#include <float.h>

#define DS1820_STAT_DATA_OK     0x01
#define DS1820_STAT_PROGRESS    0x02

#define DS1820_CONVERSION_DURATION_MILLIS     850

#define B_DS1820_OUT_CELSIUS     0
#define B_DS1820_OUT_FAHRENHEIT  1


/** Temperature sensor DS1820.
 * This class provides temperatue measurement from DS1820. It does not block microcontroller's code execution while DS1820 conversion is in progress.
 * After successfull reading, the value is available in \c output[0].
 * The measurement cycle is divided to two phases: \b conversion and \b reading from sensor.
 * After conversion issue the sensor needs at least 750 milliseconds to finish.
 * Then the result can be read.\n
 * BSensorDS1820 can be used in two modes; \b singleshot or \b periodic.\n
 * In \b singleshot mode \c (repeat_seconds = 0) every temperature measurement is started by \c trigger() method.\n
 * If \c repeat_seconds is greater than 0, BSensorDS1820 starts in \b periodic mode.\n
 * After every successfull tempreature conversion and reading, the value is stored to \c output[0]
 * and \c B_EVT_TEMPERATURE event is signaled.
 * If \c job_follow is set, BSensorDS1820 will call \c BlockSpider(job_follow) to trigger following Block.
 */
class BSensorDS1820 : public BlockBase
{
private:
    BlockSystem* system;
    OneWire* DS1820;
    BlockTimer* my_timer;
    int8_t status;
    unsigned long conv_start_millis;
    uint8_t conv_restart_sec;    //repeat conversion after so many seconds

public:
    float output[1];                  //temperature °C

	/** Constructor.
     *  BSensorDS1820 needs the BlockSystem to use events and timer.
	 *  @param [in] arduino_pin The Arduino pin number, where DS1820 sensor signal is connected.
	 *  @param [in] system Pointer to BlockSystem object.
	 *  @param [in] repeat_seconds Measurement period in seconds. If \c 0 it will start in singleshot mode.
	 */
    BSensorDS1820(uint8_t arduino_pin, BlockSystem* system, uint16_t repeat_seconds = 0);
private:
    int8_t startConversion(void);
    int8_t dataReady(void);
public:
    /** Start DS1820 measurement.*/
    bool trigger(uint8_t type = B_TRIGG_JOB); //Issue new conversion.
    /** Used by BlockSystem timer.
     * Used for temperature conversion timeout and automatic measurement restart.*/
    void onTimer(void);                       //Automatic reading, triggering and repeating. BlockSystem must be provided.
};
#endif
