#pragma once
#include <cmath>
#include <vector>
#include <map>
#include <set>

/* timeline structure */
template <typename T, typename V>
struct TimeLine {
protected:
	std::map<T,V> data;
	V none;

	typedef std::map<T,V>::iterator Iterator;
	typedef std::map<T,V>::reverse_iterator IteratorRev;
	typedef std::vector<std::pair<T,V>> SubVector;
	Iterator iteratorAt(T time) {
		Iterator itr = data.upper_bound(time);
		if (itr != data.begin()) { itr--; }
		return itr;
	}

public:
	void set(T time, V value) {
		data.insert_or_assign(time, value);
	}
	void remove(T time) {
		data.erase(time);
	}
	
	void setNext(T time, V value) {
		Iterator itr = iteratorAt(time);
		while(itr != data.end() && itr->first == time) {
			time = std::nextafter(time,INFINITY);
			itr++;
		}
		set(time,value);
	}

	void setNone(V value) { none = value; }

	int  size () { return data.size();  }
	bool empty() { return data.empty(); }




	std::pair<T,V> pairAt(T time) {
		if (empty()) { return {0,none}; }
		Iterator itr = iteratorAt(time);
		return {itr->first, itr->second};
	}

	T timeAt(T time) { return pairAt(time).first; }
	V valAt (T time) { return pairAt(time).second; }

	T  start() { return data. begin()->first; }
	T  end  () { return data.rbegin()->first; }

	V& first() { return data. begin()->second; }
	V& last () { return data.rbegin()->second; }

	

	void iterate(auto&& f) {
		for (auto [time, value] : data) {
			f(time, value);
	} }

	void iterateRev(auto&& f) {
		for (IteratorRev itr=data.rbegin(); itr != data.rend(); itr++ ) {
			f(itr->first, itr->second);
	} }

	
};



/* valueless timeline structure */
template <typename T>
struct TimeLine<T,void> {
protected:
	std::set<T> data;

	typedef std::set<T>::iterator Iterator;
	typedef std::set<T>::reverse_iterator IteratorRev;
	Iterator iteratorAt(T time) {
		Iterator itr = data.upper_bound(time);
		if (itr != data.begin()) { itr--; }
		return itr;
	}

public:
	void set(T time) {
		data.insert(time);
	}
	void remove(T time) {
		data.erase(time);
	}
	
	void setNext(T time) {
		Iterator itr = iteratorAt(time);
		while(itr != data.end() && *itr == time) {
			time = std::nextafter(time,INFINITY);
			itr++;
		}
		set(time);
	}

	int  size () { return data.size();  }
	bool empty() { return data.empty(); }



	T timeAt(T time) {
		if (empty()) { return 0; }
		Iterator itr = iteratorAt(time);
		return itr->first;
	}

	T start() { return *data. begin(); }
	T end  () { return *data.rbegin(); }



	void iterate(auto&& f) {
		for (T time : data) { f(time); }
	}

	void iterateRev(auto&& f) {
		for (IteratorRev itr=data.rbegin(); itr != data.rend(); itr++ ) {
			f(*itr);
	} }
};




/* These 3 functions were unused and belong in */
/* the NoteSteam class (not in this proect)    */

// // For animations. 'eventDomain' has the start and end times
// // when the animation plays, where 0 is the event.
// std::pair<T,T> eventDomain = {0,0};
// // 'eventProgress' gives you an ordered list of times relative to
// // each animation. (ordered chronologicaly)
// SubVector eventProgress(T time) {
// 	T begin = time - eventDomain.second;
// 	T end   = time - eventDomain.first;

// 	SubVector output = eventsInRange(begin, end);
// 	for (std::size_t i=0; i < output.size(); i++) {
// 		output[i].first = time - output[i].first;
// 	}
// 	return output;
// }

// SubVector eventsInRange(T t0, T t1) {
// 	Iterator begin = iteratorAt(t0);
// 	Iterator end   = iteratorAt(t1); end++;

// 	SubVector output;
// 	for (Iterator itr=begin; itr != end; itr++) {
// 		std::pair<T,V> event = {itr->first, itr->second};
// 		if (t0 <= event.first&&event.first <= t1) {
// 			output.push_back(event);
// 	} }
// 	return output;
// }