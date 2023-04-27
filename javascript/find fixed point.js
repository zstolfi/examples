const MAX_TRIES = 1000000;
const offset = [1,1,1,1,1,1,1,1,1,1];

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

function count(list) {
	let result = [0,0,0,0,0,0,0,0,0,0];
	for (let number of list) {
		for (let digit of number.toString()) {
			result[parseInt(digit)]++;
	} }
	// optional offset
	for (let i=0; i<10; i++) { result[i] += offset[i]; }
	return result;
}

let t = [0,0,0,0,0,0,0,0,0,0];
let list = [];
let loops = [];

function compare(c, d) {
	let index = [0,1,2,3,4,5,6,7,8,9].find( (i) => c[i] != d[i] );
	if (index == undefined) { return 0; }
	return c[index] - d[index];
}

function indexOf(c) {
	return list.findIndex( (d) => compare(c,d) == 0 );
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

let tries = 0;
for (let t0=[0,0,0,0,0,0,0,0,0,0]; ; t0[0]++) {
	for (let i=0; i<10; i++) {
		if (t0[i] >= 10) {
			let digit = t0[i] % 10;
			let carry = (t0[i] - digit) / 10;
			t0[i] = digit;
			t0[i+1] += carry;
		}
	}
	for (let i=0; i<10; i++) { t[i] = t0[i]; }

	let loopIndex;
	while ((loopIndex = indexOf(t)) == -1) {
		list.push(t);
		t = count(t);
	} /**/ {
		let loopLength = list.length - loopIndex;
		// store the unique 'minimum' value of the loop, just to prevent duplicate entires
		let min = [Infinity,Infinity,Infinity,Infinity,Infinity,Infinity,Infinity,Infinity,Infinity,Infinity];
		for (let i=loopIndex; i<list.length; i++) {
		    if (compare(list[i], min) < 0) { min = list[i]; }
		}
		if (!loops.find( (loop) => compare(loop.c, min) == 0 )) {
			loops.push({c: min, length: loopLength});
			if (loopLength == 1) { console.log("Fixed point found!", min); }
		}
		// console.log(list);
		list = [];
		if (tries++ > MAX_TRIES) { break; }
	}
}
console.log(`Search complete. ${loops.length} loops found.`);
console.log(loops);