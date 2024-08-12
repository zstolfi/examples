#include <iostream>
using UINT = unsigned;

struct Date {
	UINT year;
	UINT month;
	UINT day;
	auto operator<=>(const Date&) const = default;

	Date(UINT y, UINT m, UINT d) : year{y}, month{m}, day{d} {}
	Date(UINT daysSinceEpoch) {
		day = daysSinceEpoch + 1;
		for (year=1900;; year++) {
			UINT d = daysInYear[isLeapYear(year)];
			if (day > d) day -= d;
			else break;
		}
		for (month=1; month<=12; month++) {
			UINT d = daysInMonths[isLeapYear(year)][month];
			if (day > d) day -= d;
			else break;
		}
	}

	inline constexpr bool isLeapYear(UINT  y) {
		return y%4 == 0 && !(y%100 == 0) || y%400 == 0;
	}
	static constexpr UINT daysInYear[2] = {365, 366};
	static constexpr UINT daysInMonths[2][13] = {
		{{}, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
		{{}, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	};
};

int main () {
	UINT sum = 0;
	UINT days = 0; // Days since Mon. Jan 1st, 1900 epoch.
	for (; Date {days} != Date {1901, 1, 1}; days++) ;
	for (; Date {days} != Date {2001, 1, 1}; days++) {
		if (Date{days}.day == 1 && days%7 == 6) {
			Date date {days};
			std::cout << date.year << "/" << date.month << "/" << date.day << "\n";
			sum++;
		}
	}
	std::cout << "Sum: " << sum << "\n";
}
