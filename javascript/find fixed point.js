function count(list) {
	let result = [0,0,0,0,0,0,0,0,0,0];
	for (let number of list) {
		for (let digit of number.toString()) {
			result[parseInt(digit)]++;
	} }
	return result;
}

let tries = [];
let t = [0,0,0,0,0,0,0,0,0,0];

function indexOf(c) {
	let result = -1;
	for (let i=0; i<tries.length; i++) {
		let match = true;
		for (let j=0; j<10; j++) {
			if (c[j] != tries[i][j]) { match = false; }
		}
		if (match) { return i; }
	}
	return result;
}

while (indexOf(t) == -1) {
	tries.push(t);
	t = count(t);
}

console.log(tries);