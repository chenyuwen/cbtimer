#ifndef __CBTIMER_H__
#define __CBTIMER_H__

//////////////////////////////宏定义////////////////////////////// 
#define CBTIMER_SIZE 5

#define _DBG_CT_
#ifdef _DBG_CT_
	#define DBG_TIMER(format,...) printf("File: "__FILE__", Line: %05d: "format"/n", __LINE__, ##__VA_ARGS__)  
#else /*_DBG_CT_*/
	#define DBG_TIMER(format, ...)   do{}while(0)
#endif /*_DBG_CT_*/



/////////////////////////////数据定义//////////////////////////////
typedef struct cbtimer {
	struct list_head list;
	
	void (*cbhandler)(struct cbtimer *, void *arg);
	
	unsigned long reduce_tick;
	
	void *arg;
}cbtimer_t;
typedef void (*cbhandler_t)(struct cbtimer *, void *arg);

////////////////////////////函数声明///////////////////////////// 
struct cbtimer *cbtimer_first_entry(struct list_head *head);
void cbtimer_irq(unsigned long reduce);
void cbtimer_insert_ready(struct cbtimer *insert, unsigned long delay);
struct cbtimer *cbtimer_pop_wait(void);
void cbtimer_init(void); 


////////////////////////////////////////////////////////////////
#endif /*__CBTIMER_H__*/


