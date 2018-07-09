/* 
 * 
*/

#ifndef BlockBase_h
#define BlockBase_h

#include <stddef.h>
#include <inttypes.h>
#include <math.h>     //NAN definion
#include "Blocks.h"


#define B_STATUS_OK 0

#define B_PARAM_VALUE_BAD   NAN

// #define B_name_IPAR_descr   0 //input param
// #define B_name_IPAR_descr   1
// #define B_name_IPAR_descr   2

// #define B_name_OPAR_descr   0 //output param
// #define B_name_OPAR_descr   1
// #define B_name_OPAR_descr   2

// #define B_name_SPAR_descr   0 //setting param
// #define B_name_SPAR_descr   1
// #define B_name_SPAR_descr   2

/* TRIGGER TYPES */
#define B_TRIGG_FALSE           0  //no trigger issued
#define B_TRIGG_JOB             1  //job issue only

/** Base class for all Blocks.
 *  It represents a common interface for Blocks interoperability and connectivity to common Blocks system.
 *  It can not be used standalone. It has to be inherited by some final class.
 */
class BlockBase
{
protected:
public:
    //float* input[n];
    //float output[n];
    //float setting[n];
    
	/** Pointer to the Block whos job is next to be executed. */
    BlockBase* job_follow;          //next job to trigger by BlockSpider()
	/** Pointer to the following Block in the system's event chain. */
    BlockBase* next_event_listener; //used for 'event' propagation through the chain
  	/** Register containing event flags to be observed (caught) by the derived Block. */
    event_t catching_events;


	/** Constructor. */
    BlockBase(void);
	
    /** Executes the job of derived Block.
     *  This method is virtual and supposed to be overriden by derived class.
	 *  The method is called by other Block through BlockSpider mechanism or by timer, owned by the Block.
     *  Derived class is responsible to implement this method in the following way:
     *  It executes its job.
     *  If executed job is finished instantly, the method returns TRUE.
     *  If job will be finished later, then method returns FALSE.
     * (This is the case of jobs like temperature sensor conversion/reading or some time dependent processes.)
	 *  @param [in] type Additional possible parameter for calling the job.
	 *  @return
	 *	- true if job is finished instantly
	 *	- false if job will be finished later
	 */
    virtual bool trigger(uint8_t type = B_TRIGG_JOB);     //Let the Block do his job. Returns true if job instantly finished, else false.
    
    /** OnTimer event handler of derived block.
     *  This method is virtual and supposed to be overriden by derived class.
     *  When derived block uses the BlockSystem timer, this method will be called after timer timeout.
     */
    virtual void onTimer(void);                           //synchronize block with some singleshot or periodic 'timer'
    
    /** Event handler of derived class.
     *  This method is virtual and supposed to be overriden by derived class.
     *  When derived block is bound to the BlockSystem event chain, this method will be called on every event.
     *  The event_flags will be passed, to enable the handler to catch the proper event.
     */
    virtual void onEvent(event_t event_flags);            //'event listener'
};


void BlockSpider(BlockBase* at);

#endif
