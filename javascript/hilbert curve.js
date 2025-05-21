const tf = (v, m) => ({
	x: m[0]*v.x + m[1]*v.y + m[2],
	y: m[3]*v.x + m[4]*v.y + m[5]
});

const flipCase = (v) => {
	let {x,y} = v;
	while (x > 1 || y > 1) x >>= 2, y >>= 2;
	return x || y;
}

const snapDown = (x) => {
	// Remove all but the MSB (Kernighan's Algorithm)
	while (x & x-1) x &= x-1;
	return x;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

// [0, 1] -> [0, 1] × [0, 1]
const hilbertUnit = (i, t) => {
	if (t < 0) return hilbert(i, 0);
	if (t > 1) return hilbert(i, 1);
	if (t == 1) return {x:1, y:0};
	if (i == 0) return {x:0, y:0};
	const u = 4*t;
	if (0 <= u&&u < 1) {
		const h = hilbert(i-1, u-0);
		return {x: 0.5 * h.y, y: 0.5 * h.x};
	}
	if (1 <= u&&u < 2) {
		const h = hilbert(i-1, u-1);
		return {x: 0.5 * h.x, y: 0.5 * (h.y+1)};
	}
	if (2 <= u&&u < 3) {
		const h = hilbert(i-1, u-2);
		return {x: 0.5 * (h.x+1), y: 0.5 * (h.y+1)};
	}
	if (3 <= u&&u < 4) {
		const h = hilbert(i-1, u-3);
		perp({x:1, y:1} - hilbert(i-1, u-3)) + {x:1, y:0};
		return {x: 0.5 * ((1-1/Math.pow(2,i)) - h.y + 1)
		,       y: 0.5 * ((1-1/Math.pow(2,i)) - h.x)};
	}
};

// [0, 1] × [0, 1] -> [0, 1]
const hilbertUnit_inv = (i, v) => {
	if (i == 0) return 0;
	const qx = v.x >= 0.5;
	const qy = v.y >= 0.5;
	
	let q = 0;
	if (!qx && !qy) q = 0;
	if (!qx &&  qy) q = 1;
	if ( qx &&  qy) q = 2;
	if ( qx && !qy) q = 3;

	const c = [
		[ 0, 2, 0 ,  2, 0, 0],
		[ 2, 0, 0 ,  0, 2,-1],
		[ 2, 0,-1 ,  0, 2,-1],
		[ 0,-2, 1 , -2, 0, 2]
	];

	const p = tf(v, c[q]);
	return 1/4 * (q + hilbert_inv(i-1, p));
};

// [0, 4^N) -> [0, 2^N) × [0, 2^N)
const hilbert = (t) => {
	let c = false;
	let S = 1, s = 0;
	result = {x:0, y:0};
	for (;t; t >>= 2, c = !c) {
		S *= 2, s = S/2;

		if (t%4 == 0) continue;
		if (t%4 == 1) result = tf(result, [ 0, 1, c? s : 0  ,  1, 0, c? 0 : s ]);
		if (t%4 == 2) result = tf(result, [ 0, 1, c? s : s  ,  1, 0, c? s : s ]);
		if (t%4 == 3) result = tf(result, [-1, 0, c?s-1:S-1 ,  0,-1, c?S-1:s-1]);
	}
	return result;
};

// [0, 2^N) × [0, 2^N) -> [0, 4^N)
const hilbert_inv = (v) => {
	let {x,y} = v;
	let result = 0;
	while (x || y) {
		let c = flipCase({x:x, y:y});
		let s = Math.max(snapDown(x), snapDown(y));
		let S = 2*s;
		let qx = x >= s; let qy = y >= s;

		let quadrant = 0;
		if (!qx &&  qy) quadrant = c? 3: 1;
		if ( qx &&  qy) quadrant = c? 2: 2;
		if ( qx && !qy) quadrant = c? 1: 3;

		w = {x:x, y:y};
		if (quadrant == 1) w = tf(w, [ 0, 1, c? 0 :-s  ,  1, 0, c?-s : 0 ]);
		if (quadrant == 2) w = tf(w, [ 0, 1, c?-s :-s  ,  1, 0, c?-s :-s ]);
		if (quadrant == 3) w = tf(w, [-1, 0, c?s-1:S-1 ,  0,-1, c?S-1:s-1]);
		x=w.x, y=w.y;

		result += s*s * quadrant;
	}
	return result;
};

// iteration, bitCount, prefix -> [xy-min, xy-max)
const hilbertBitRect = (i, b, n) => {
	let t0 = n << (i-b);
	let t1 = (n+1) << (i-b);
	let hn = [
		hilbert(t0),
		hilbert(Math.floor((2*t0 + t1) / 3)),
		hilbert(Math.floor((t0 + 2*t1) / 3)),
		hilbert(t1-1),
	];
	let hx = hn.map(h => h.x);
	let hy = hn.map(h => h.y);
	if (i%4 >= 2) [hx, hy] = [hy, hx];

	return {
		min: {x: Math.min(...hx)  , y: Math.min(...hy)  },
		max: {x: Math.max(...hx)+1, y: Math.max(...hy)+1}
	};
};
