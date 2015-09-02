#include "cpu.h"
#include "list_gnu.h" //#include "list.h"
#include "cbtimer.h"

LIST_HEAD(ready);  
LIST_HEAD(wait);

static struct cbtimer cbwait[CBTIMER_SIZE] = {0};

/*
//////////////////////////////////////////////// 
*             取得定时器的头节点 
* 
* 说明： 获得头节点，但不删除 
* 参数： head 链表头 
* 返回值： 获取的节点 
*
///////////////////////////////////////////////
*/ 
struct cbtimer *cbtimer_first_entry(struct list_head *head)
{
	struct cbtimer *ret = NULL;
	if(!list_empty(head)) {
		ret = list_first_entry(head, struct cbtimer, list);
	}
	
	return ret;
}

/*
//////////////////////////////////////////////// 
*             软件定时器的中断处理函数 
* 
* 说明： 该函数由中断处理函数进行调用，每一个
*        时钟周期调用一次。
* 参数： reduce 此时系统的时钟滴答的计数值 
*
*
/////////////////////////////////////////////// 
*/
void cbtimer_irq(unsigned long reduce)
{
	struct cbtimer *timer;
	psr_t psr;

	psr = ENTER_CRITICAL();
	timer = cbtimer_first_entry(&ready);
	
	if(timer == NULL) {
		EXIT_CRITICAL(psr);
		return;
	}

	if(timer->reduce_tick <= reduce) {//计时到达 
		list_del(&timer->list);
		EXIT_CRITICAL(psr);
		
		if(timer->cbhandler != NULL)
			timer->cbhandler((void *)timer, timer->arg);
		
		psr = ENTER_CRITICAL();
		list_add(&timer->list, &wait);
		EXIT_CRITICAL(psr); 
	} else {
		timer->reduce_tick -= reduce;
		EXIT_CRITICAL(psr); 
	}
}

/*
//////////////////////////////////////////////// 
*             定时器插入就绪链表 
* 
* 说明： 把定时器插入就绪链表 
* 参数： insert 需要插入的定时器 
*        delay 定时值 
*
/////////////////////////////////////////////// 
*/
void cbtimer_insert_ready(struct cbtimer *insert, unsigned long delay)
{
	struct list_head *pos;
	struct cbtimer *tmp;
	psr_t psr;
	
	psr = ENTER_CRITICAL();
	list_for_each_prev(pos, &ready) {
		tmp = list_entry(pos, struct cbtimer, list);
		
		if(delay > tmp->reduce_tick) {
			delay -= tmp->reduce_tick;
		} else {
			break;
		}
	}
	
	insert->reduce_tick = delay;
	list_add_tail(&insert->list, &ready);
	EXIT_CRITICAL(psr); 
}

/*
//////////////////////////////////////////////// 
*             从空闲链表中取出 
* 
* 说明： 从空闲链表中取出空闲定时器 
* 返回值：获得的空闲的定时器 
*
/////////////////////////////////////////////// 
*/
struct cbtimer *cbtimer_pop_wait()
{
	struct cbtimer *ret = NULL;
	psr_t psr;
	
	psr = ENTER_CRITICAL();
	ret = cbtimer_first_entry(&wait);
	if(ret != NULL) {
		list_del(&ret->list);
	}
	EXIT_CRITICAL(psr); 
	return ret;
} 

/*
//////////////////////////////////////////////// 
*            cbtimer初始化函数 
* 
* 说明： 把全局的cbwait加入到wait链表中 
*
/////////////////////////////////////////////// 
*/
void cbtimer_init()
{
	int i = 0;
	for(i=0; i<CBTIMER_SIZE; i++) {
		list_add(&(cbwait[i].list), &wait);
	}
}

/*
//////////////////////////////////////////////// 
*            cbtimer注册函数 
* 
* 说明： 注册cbtimer
* 参数： handler: 处理者
*        arg:     handler的参数
*        delay:   定时值 
*
/////////////////////////////////////////////// 
*/
int cbtimer_register(cbhandler_t handler, void *arg, int delay)
{
	struct cbtimer *tmp;
	tmp = cbtimer_pop_wait();

	if(tmp == NULL) {
		return -1;
	}
		
	tmp->cbhandler = handler;
	tmp->arg = arg;
		
	cbtimer_insert_ready(tmp, delay);
	
	return 0;
}









