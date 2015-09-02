#include <stdio.h>
#include <stdlib.h>
#include "list_gnu.h"  //#include "list.h"
#include "cbtimer.h"

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

void handler(struct cbtimer *timer, void *arg)
{
	unsigned long tmp = (void *)arg;
	
	printf("handle timer %d\n", tmp);
}


int main(int argc, char *argv[]) 
{
	int i = 0;
	struct cbtimer *tmp;
	cbtimer_init();
	
	cbtimer_register(handler, (void *)1, 1);
	cbtimer_register(handler, (void *)6, 6);
	cbtimer_register(handler, (void *)20, 20);
	
	while(1) {
		cbtimer_irq(1);
		system("pause");
	}
	
	return 0;
}

