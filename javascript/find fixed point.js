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
	return undefined == [0,1,2,3,4,5,6,7,8,9].find( (i) => c[i] != d[i] );
}

function indexOf(c) {
	return tries.findIndex( (d) => compare(c,d) );
}



t = [0,0,0,0,0,0,0,0,0,0];

while (indexOf(t) == -1) {
	tries.push(t);
	t = count(t);
} /**/ {
	loops.push({c: t, length: tries.length - indexOf(t)});
}

console.log(loops);