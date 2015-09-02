#include "cpu.h"
#include "list_gnu.h" //#include "list.h"
#include "cbtimer.h"

LIST_HEAD(ready);  
LIST_HEAD(wait);

static struct cbtimer cbwait[CBTIMER_SIZE] = {0};

/*
//////////////////////////////////////////////// 
*             ȡ�ö�ʱ����ͷ�ڵ� 
* 
* ˵���� ���ͷ�ڵ㣬����ɾ�� 
* ������ head ����ͷ 
* ����ֵ�� ��ȡ�Ľڵ� 
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
*             �����ʱ�����жϴ����� 
* 
* ˵���� �ú������жϴ��������е��ã�ÿһ��
*        ʱ�����ڵ���һ�Ρ�
* ������ reduce ��ʱϵͳ��ʱ�ӵδ�ļ���ֵ 
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

	if(timer->reduce_tick <= reduce) {//��ʱ���� 
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
*             ��ʱ������������� 
* 
* ˵���� �Ѷ�ʱ������������� 
* ������ insert ��Ҫ����Ķ�ʱ�� 
*        delay ��ʱֵ 
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
*             �ӿ���������ȡ�� 
* 
* ˵���� �ӿ���������ȡ�����ж�ʱ�� 
* ����ֵ����õĿ��еĶ�ʱ�� 
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
*            cbtimer��ʼ������ 
* 
* ˵���� ��ȫ�ֵ�cbwait���뵽wait������ 
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
*            cbtimerע�ắ�� 
* 
* ˵���� ע��cbtimer
* ������ handler: ������
*        arg:     handler�Ĳ���
*        delay:   ��ʱֵ 
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









