#include "os_core.h"

DateTime LocalTime = {16,5,3,10,50,true};

void OS_TimeRefresh(void)
{
	if((LocalTime.year%4==0)&&(LocalTime.year%100!=0)||(LocalTime.year%400==0))
		LocalTime.is_leap_year = true;
	else
		LocalTime.is_leap_year = false;
		
	if(++LocalTime.second == 60)
	{
		LocalTime.second = 0;
		if(++LocalTime.min == 60)
		{
			LocalTime.min = 0;
			if(++LocalTime.hour == 60)
			{
				LocalTime.hour = 0;
				if(++LocalTime.hour == 24)
				{
					LocalTime.hour = 0;
					if(LocalTime.month == 2)
					{
						if(LocalTime.is_leap_year == true)
						{
							if(++LocalTime.day == 30)
							{
								LocalTime.day = 0;
								++LocalTime.month;
							}
						}
						else
						{
							if(++LocalTime.day == 29)
							{
								LocalTime.day = 0;
								++LocalTime.month;
							}
						}
					}
					else if(LocalTime.month == 1 || \
							LocalTime.month == 3 || \
							LocalTime.month == 5 || \
							LocalTime.month == 7 || \
							LocalTime.month == 8 || \
							LocalTime.month == 10 || \
							LocalTime.month == 12)
					{
						if(++LocalTime.day == 32)
						{
							LocalTime.day = 0;
							if(++LocalTime.month == 13)
							{
								LocalTime.month = 0;
								LocalTime.year++;
							}
						}
					}
					else
					{
						if(++LocalTime.day == 31)
						{
							LocalTime.day = 0;
							if(++LocalTime.month == 13)
							{
								LocalTime.month = 0;
								LocalTime.year++;
							}
						}
					}
				}
			}
		}
	}
}



