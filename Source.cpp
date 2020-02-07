#include<stdio.h>
#include "Tree.h"

void main()
{
	char buf[256];
	while (1)
	{
		gets_s(buf);
		Tree t(buf);
	
		int err = t.Evaluate();
		if (err == 0)
		{
			printf("%s = %d\n", buf, t.GetResult());
		}
		else
		{
			switch (err)
			{
			case 2:
				printf("Error - should start with digit\n");
				break;
			case 3:
				printf("Error - operator came after operator\n");
				break;
			case 4:
				printf("Error - literal is too large\n");
				break;
			case 5:
				printf("Error - closing bracket without opening\n");
				break;
			case 10:
				printf("Error - either negative literal or unary minus\n");
				break;
			case -1:
				printf("Error - wrong literal \n");
			}
		}
	}
	
}