const tf = (v, m) => ({
	x: m[0]*v.x + m[1]*v.y + m[2],
	y: m[3]*v.x + m[4]*v.y + m[5]
});

// [0, 1] version
const hilbert = (i, t) => {
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

// [0, 4^N) version
const hilbert = (i, t) => {
	const N = 1 << (2*i), n = N / 4;
	const S = 1 << i,     s = S / 2;
	if (t < 0) return hilbert(i, 0);
	if (t >= N) return hilbert(i, N-1);
	if (i == 0) return {x:0, y:0};

	const tf = (v, mx, my) => ({
		x: mx[0]*v.x + mx[1]*v.y + mx[2],
		y: my[0]*v.x + my[1]*v.y + my[2]
	});

	const h = hilbert(i-1, t%n);
	switch (Math.floor(t/n)) {
		case 0: return tf(h, [ 0, 1, 0  ,  1, 0, 0  ]);
		case 1: return tf(h, [ 1, 0, 0  ,  0, 1, s  ]);
		case 2: return tf(h, [ 1, 0, s  ,  0, 1, s  ]);
		case 3: return tf(h, [ 0,-1, S-1, -1, 0, s-1]);
	}
};

// unit square version
const hilbert_inv = (i, v) => {
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

// 2^N x 2^N square version
const hilbert_inv = (i, v) => {
	const N = 1 << (2*i), n = N / 4;
	const S = 1 << i,     s = S / 2;
	if (i == 0) return 0;
	
	let q = 0;
	const [x, y] = [v.x >= s, v.y >= s];
	if (!x && !y) q = 0;
	if (!x &&  y) q = 1;
	if ( x &&  y) q = 2;
	if ( x && !y) q = 3;

	const c = [
		[ 0, 1, 0  ,  1, 0, 0  ],
		[ 1, 0, 0  ,  0, 1,-s  ],
		[ 1, 0,-s  ,  0, 1,-s  ],
		[ 0,-1, s-1, -1, 0, S-1]
	];

	const p = tf(v, c[q]);
	return q*n + hilbert_inv(i-1, p);
};
