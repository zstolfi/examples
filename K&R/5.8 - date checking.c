/* Exercise 5-8.
	There is no error checking in day_of_year or month_day.
	Remedy this defect.
*/

#include <stdbool.h>

static char daytab[2][13] = {
	{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

/* return number of days so far */
int day_of_year(int year, int month, int day) {
	bool leap = year%4 == 0 && year%100 != 0
	         || year%400 == 0;

	/* Given the style of error-return in month_name   
	   returning NaN-style values is what I'll copy. */
	if (!(1 <= month && month <= 12))
		return -1;
	if (!(1 <= day && day <= daytab[leap][month]))
		return -1;

	int i = 1;
	for (; i < month; i++)
		day += daytab[leap][i];
	return day;
}

/* heavens no, cannot return multiple values :( */
void month_day(int year, int yearday, int *pmonth, int *pday) {
	bool leap = year%4 == 0 && year%100 != 0
	         || year%400 == 0;

	if (!(1 <= yearday && yearday <= 365 + leap)) {
		*pmonth = *pday = -1;
		return;
	}

	int i = 1;
	for (; yearday > daytab[leap][i]; i++)
		yearday -= daytab[leap][i];
	*pmonth = i;
	*pday = yearday;
}

char *month_name(int n) {
	static char *name[] = {
		"Illegal month",
		"January", "February", "March", "April",
		"May", "June", "July", "August",
		"September", "October", "November", "December"
	};
	return !(1 <= n&&n <=12) ? name[0] : name[n];
}



#include <stdio.h>

int main(void) {
	printf("Day of year for Oct. 31, 2020: %d\n"
	      ,day_of_year(2020, 10, 31));
	printf("Day of year for Oct. 31, 2021: %d\n"
	      ,day_of_year(2021, 10, 31));

	int month, day;
	month_day(1950, 200, &month, &day);

	printf("200th day of 1950: %s. %d\n"
	      ,month_name(month), day);

	return 0;
}