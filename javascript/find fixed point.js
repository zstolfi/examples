function count(list) {
	let result = [0,0,0,0,0,0,0,0,0,0];
	for (let number of list) {
		for (let digit of number.toString()) {
			result[parseInt(digit)]++;
	} }
	return result;
}

let t;
let tries = [];
let loops = [];

function compare(c, d) {
	let index = [0,1,2,3,4,5,6,7,8,9].find( (i) => c[i] != d[i] );
	if (index == undefined) { return 0; }
	return c[index] - d[index];
}

function indexOf(c) {
	return tries.findIndex( (d) => compare(c,d) == 0 );
}


// It seems there are only 2 possible loops in base 10
// https://www.youtube.com/watch?v=1GKfEDvhWdY
t0 = [0,0,0,0,0,0,0,0,0,0];
while (loops.length < 2) {
	t0[0]++;
	for (let i=0; i<10; i++) {
		if (t0[i] >= 10) {
			let digit = t0[i] % 10;
			let carry = (t0[i] - digit) / 10;
			t0[i] = digit;
			t0[i+1] += carry;
		}
	}
	t = t0;

	let loopIndex;
	while ((loopIndex = indexOf(t)) == -1) {
		tries.push(t);
		t = count(t);
	} /**/ {
		let loopLength = tries.length - loopIndex;
		// store the unique 'minimum' value of the loop, just to prevent duplicate entires
		let min = [Infinity,Infinity,Infinity,Infinity,Infinity,Infinity,Infinity,Infinity,Infinity,Infinity];
		for (let i=loopIndex; i<tries.length; i++) {
		    if (compare(tries[i], min) < 0) { min = tries[i]; }
		}
		if (!loops.find( (loop) => compare(loop.c, min) == 0 )) {
			loops.push({c: min, length: loopLength});
			if (loopLength == 1) { console.log("Fixed point found!", min); }
		}
		tries = [];
	}
}
console.log(loops);